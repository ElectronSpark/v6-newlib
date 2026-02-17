/*
 * utelnetd.c — Userspace telnet server for xv6
 *
 * Uses the PTY devtmpfs interface (/dev/ptmx + /dev/pts/N) and BSD
 * sockets to provide remote shell access entirely from userspace.
 *
 * Architecture:
 *   main (listener)
 *     │  accept()
 *     ▼
 *   session_handler (forked per connection)
 *     ├── open("/dev/ptmx") → master_fd
 *     ├── ioctl(master_fd, TIOCGPTN) → pts index N
 *     ├── fork()
 *     │   └── child: setsid, open /dev/pts/N as 0/1/2, exec /bin/sh
 *     └── parent: bridge TCP ↔ PTY master (telnet protocol)
 *
 * Build: newlib program via add_newlib_program() in CMakeLists.txt
 * Run:   $ utelnetd &      (from xv6 shell)
 *        From host: telnet localhost 2323
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <poll.h>
#include <fcntl.h>
#include <netinet/tcp.h>

/* ── Telnet protocol constants (RFC 854/855) ─────────────────────── */

#define IAC   255
#define DONT  254
#define DO    253
#define WONT  252
#define WILL  251
#define SB    250
#define SE    240

#define TELOPT_ECHO   1
#define TELOPT_SGA    3
#define TELOPT_NAWS   31
#define TELOPT_LMODE  34

/* ── Configuration ───────────────────────────────────────────────── */

#define LISTEN_PORT   23
#define LISTEN_BACKLOG 4
#define BUF_SIZE      512

/* ── Telnet initial negotiation ──────────────────────────────────── */

static void send_negotiation(int sock) {
    unsigned char neg[] = {
        IAC, WILL, TELOPT_ECHO,
        IAC, WILL, TELOPT_SGA,
        IAC, DO,   TELOPT_NAWS,
        IAC, DO,   TELOPT_SGA,
        IAC, DONT, TELOPT_LMODE,
    };
    write(sock, neg, sizeof(neg));
}

/* ── Telnet IAC processing ───────────────────────────────────────── */

/*
 * Process a telnet IAC command.  Returns bytes consumed (including IAC).
 * Handles WILL/WONT/DO/DONT and subnegotiation (NAWS window size).
 * If master_fd >= 0, NAWS updates are forwarded via TIOCSWINSZ ioctl.
 */
static int process_iac(const unsigned char *buf, int len, int master_fd) {
    if (len < 2)
        return 0;

    unsigned char cmd = buf[1];

    switch (cmd) {
    case IAC:
        return 2;  /* escaped 0xFF — caller handles */

    case WILL:
    case WONT:
    case DO:
    case DONT:
        if (len < 3)
            return 0;
        return 3;

    case SB:
        /* Subnegotiation — scan for IAC SE */
        for (int i = 2; i < len - 1; i++) {
            if (buf[i] == IAC && buf[i + 1] == SE) {
                /* NAWS: SB NAWS <4 bytes> IAC SE */
                if (len >= 9 && buf[2] == TELOPT_NAWS && master_fd >= 0) {
                    struct winsize ws;
                    ws.ws_col = ((unsigned)buf[3] << 8) | buf[4];
                    ws.ws_row = ((unsigned)buf[5] << 8) | buf[6];
                    ws.ws_xpixel = 0;
                    ws.ws_ypixel = 0;
                    ioctl(master_fd, TIOCSWINSZ, &ws);
                }
                return i + 2;
            }
        }
        return 0;  /* incomplete */

    default:
        return 2;
    }
}

/*
 * Strip telnet IAC commands from raw TCP data, writing clean user
 * data to out[].  Returns number of user-data bytes.
 */
static int strip_telnet(const unsigned char *in, int inlen,
                        unsigned char *out, int outmax, int master_fd) {
    int opos = 0;
    int i = 0;
    while (i < inlen && opos < outmax) {
        if (in[i] == IAC) {
            if (i + 1 < inlen && in[i + 1] == IAC) {
                /* Escaped 0xFF → literal */
                out[opos++] = 0xFF;
                i += 2;
                continue;
            }
            int consumed = process_iac(in + i, inlen - i, master_fd);
            if (consumed == 0)
                break;  /* incomplete at end */
            i += consumed;
        } else {
            out[opos++] = in[i++];
        }
    }
    return opos;
}

/*
 * Escape outgoing data for telnet: double IAC bytes, convert LF → CR LF.
 * Returns number of bytes written to out[].
 */
static int telnet_escape(const unsigned char *in, int inlen,
                         unsigned char *out, int outmax) {
    int opos = 0;
    for (int i = 0; i < inlen && opos + 1 < outmax; i++) {
        if (in[i] == IAC) {
            out[opos++] = IAC;
            out[opos++] = IAC;
        } else if (in[i] == '\n') {
            out[opos++] = '\r';
            out[opos++] = '\n';
        } else {
            out[opos++] = in[i];
        }
    }
    return opos;
}

/* ── Open a PTY pair via devtmpfs ────────────────────────────────── */

/*
 * Opens /dev/ptmx, retrieves the slave index via TIOCGPTN, and opens
 * the corresponding /dev/pts/N.
 *
 * Returns 0 on success, -1 on failure.
 * On success: *master_fd and *slave_fd are set, pts_name is filled.
 */
static int open_pty_pair(int *master_fd, int *slave_fd,
                         char *pts_name, int pts_name_len) {
    int mfd = open("/dev/ptmx", O_RDWR);
    if (mfd < 0) {
        printf("utelnetd: open /dev/ptmx failed\n");
        return -1;
    }

    int pts_num = -1;
    if (ioctl(mfd, TIOCGPTN, &pts_num) < 0) {
        printf("utelnetd: TIOCGPTN failed\n");
        close(mfd);
        return -1;
    }

    snprintf(pts_name, pts_name_len, "/dev/pts/%d", pts_num);

    int sfd = open(pts_name, O_RDWR);
    if (sfd < 0) {
        printf("utelnetd: open %s failed\n", pts_name);
        close(mfd);
        return -1;
    }

    *master_fd = mfd;
    *slave_fd = sfd;
    return 0;
}

/* ── Spawn shell on PTY slave ────────────────────────────────────── */

/*
 * Fork a child that becomes a session leader, opens the PTY slave
 * as stdin/stdout/stderr, and exec's /bin/sh.
 *
 * The slave_fd passed in is closed in both parent and child (the child
 * re-opens the slave device after setsid).
 *
 * Returns the child PID in the parent, or -1 on error.
 */
static int spawn_shell(int slave_fd, const char *pts_name,
                       int master_fd, int sock_fd) {
    int pid = fork();
    if (pid < 0)
        return -1;

    if (pid == 0) {
        /* ── Child ── */

        /* Close inherited fds that belong to the session handler */
        close(slave_fd);
        close(master_fd);
        close(sock_fd);

        /* Create a new session — detaches from parent's controlling tty */
        setsid();

        /* Open the slave PTY as the controlling terminal */
        int fd0 = open(pts_name, O_RDWR);
        if (fd0 < 0)
            _exit(1);

        /* Set it as controlling terminal */
        ioctl(fd0, TIOCSCTTY, 0);

        /* Set up stdin/stdout/stderr */
        dup2(fd0, 0);
        dup2(fd0, 1);
        dup2(fd0, 2);
        if (fd0 > 2)
            close(fd0);

        /* Set reasonable termios defaults for telnet */
        struct termios tp;
        tcgetattr(0, &tp);
        tp.c_iflag = ICRNL;
        tp.c_oflag = OPOST | ONLCR;
        tp.c_cflag = CS8 | CREAD;
        tp.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK;
        tcsetattr(0, 0 /* TCSANOW */, &tp);

        /* Set default window size */
        struct winsize ws;
        ws.ws_col = 80;
        ws.ws_row = 24;
        ws.ws_xpixel = 0;
        ws.ws_ypixel = 0;
        ioctl(0, TIOCSWINSZ, &ws);

        /* Set foreground process group */
        pid_t mypid = getpid();
        ioctl(0, TIOCSPGRP, &mypid);

        /* Exec the shell */
        char *argv[] = {"sh", 0};
        char *envp[] = {0};
        execve("/bin/sh", argv, envp);
        _exit(127);
    }

    /* ── Parent ── */
    close(slave_fd);
    return pid;
}

/* ── Session handler (runs in forked process per connection) ─────── */

static void session_handler(int sock_fd) {
    char pts_name[32];
    int master_fd = -1, slave_fd = -1;

    /* Disable Nagle algorithm — telnet requires low-latency delivery
     * of small segments (prompt, echo chars) rather than batching. */
    {
        int nodelay = 1;
        setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY,
                   &nodelay, sizeof(nodelay));
    }

    /* Send telnet negotiation */
    send_negotiation(sock_fd);

    /* Open PTY pair */
    if (open_pty_pair(&master_fd, &slave_fd, pts_name, sizeof(pts_name)) < 0) {
        close(sock_fd);
        _exit(1);
    }

    printf("utelnetd: opened PTY %s\n", pts_name);

    /* Spawn shell */
    int shell_pid = spawn_shell(slave_fd, pts_name, master_fd, sock_fd);
    if (shell_pid < 0) {
        printf("utelnetd: fork for shell failed\n");
        close(master_fd);
        close(sock_fd);
        _exit(1);
    }

    /* slave_fd is closed by spawn_shell in parent */

    /*
     * Bridge loop: use poll() to multiplex between:
     *   - TCP socket → PTY master (strip telnet commands)
     *   - PTY master → TCP socket (escape IAC, LF → CRLF)
     */
    struct pollfd fds[2];
    fds[0].fd = sock_fd;
    fds[0].events = POLLIN;
    fds[1].fd = master_fd;
    fds[1].events = POLLIN;

    unsigned char rbuf[BUF_SIZE];
    unsigned char wbuf[BUF_SIZE * 2];

    int running = 1;
    while (running) {
        int ret = poll(fds, 2, 5000 /* 5s timeout for liveness check */);
        if (ret < 0)
            break;

        if (ret == 0) {
            /* Timeout — check if shell is still alive */
            int status;
            int w = waitpid(shell_pid, &status, WNOHANG);
            if (w > 0) {
                /* Shell exited */
                running = 0;
                break;
            }
            continue;
        }

        /* TCP → PTY master */
        if (fds[0].revents & POLLIN) {
            ssize_t n = read(sock_fd, rbuf, sizeof(rbuf));
            if (n <= 0) {
                running = 0;
                break;
            }
            /* Strip telnet commands and forward user data to pty */
            unsigned char clean[BUF_SIZE];
            int clen = strip_telnet(rbuf, n, clean, sizeof(clean), master_fd);
            if (clen > 0)
                write(master_fd, clean, clen);
        }

        if (fds[0].revents & (POLLERR | POLLHUP)) {
            running = 0;
            break;
        }

        /* PTY master → TCP */
        if (fds[1].revents & POLLIN) {
            ssize_t n = read(master_fd, rbuf, sizeof(rbuf));
            if (n <= 0) {
                running = 0;
                break;
            }
            /* Escape for telnet protocol */
            int elen = telnet_escape(rbuf, n, wbuf, sizeof(wbuf));
            if (elen > 0)
                write(sock_fd, wbuf, elen);
        }

        if (fds[1].revents & (POLLERR | POLLHUP)) {
            running = 0;
            break;
        }
    }

    /* Clean up */
    kill(shell_pid, SIGKILL);
    int status;
    waitpid(shell_pid, &status, 0);

    close(master_fd);
    close(sock_fd);

    printf("utelnetd: session ended\n");
    _exit(0);
}

/* ── SIGCHLD handler: reap zombie children ───────────────────────── */

static void sigchld_handler(int sig) {
    (void)sig;
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0)
        ;
}

/* ── Main: listener loop ─────────────────────────────────────────── */

int main(void) {
    /* Set up SIGCHLD handler to reap forked session children */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &sa, 0);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        printf("utelnetd: socket() failed\n");
        return 1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, 2 /* SO_REUSEADDR */, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LISTEN_PORT);
    addr.sin_addr.s_addr = 0; /* INADDR_ANY */

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("utelnetd: bind() failed\n");
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        printf("utelnetd: listen() failed\n");
        close(listen_fd);
        return 1;
    }

    printf("utelnetd: listening on port %d\n", LISTEN_PORT);

    for (;;) {
        struct sockaddr_in client;
        socklen_t clen = sizeof(client);
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client, &clen);
        if (conn_fd < 0) {
            printf("utelnetd: accept() failed\n");
            continue;
        }

        unsigned char *ip = (unsigned char *)&client.sin_addr.s_addr;
        printf("utelnetd: connection from %d.%d.%d.%d:%d\n",
               ip[0], ip[1], ip[2], ip[3], ntohs(client.sin_port));

        int pid = fork();
        if (pid < 0) {
            printf("utelnetd: fork() failed\n");
            close(conn_fd);
            continue;
        }

        if (pid == 0) {
            /* Child: close listener, handle session */
            close(listen_fd);
            session_handler(conn_fd);
            /* session_handler calls _exit */
        }

        /* Parent: close accepted socket, continue listening */
        close(conn_fd);
    }

    /* unreachable */
    close(listen_fd);
    return 0;
}
