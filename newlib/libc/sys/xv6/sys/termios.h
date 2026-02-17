/*
 * sys/termios.h - xv6 termios definitions for newlib
 */
#ifndef _SYS_TERMIOS_H
#define _SYS_TERMIOS_H

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* c_cc indexes */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6
#define VSTART 7
#define VSTOP 8
#define VSUSP 9
#define VEOL 10
#define NCCS 16

/* c_iflag bits */
#define IGNBRK 0x0001
#define BRKINT 0x0002
#define IGNPAR 0x0004
#define PARMRK 0x0008
#define INPCK 0x0010
#define ISTRIP 0x0020
#define INLCR 0x0040
#define IGNCR 0x0080
#define ICRNL 0x0100
#define IXON 0x0200
#define IXOFF 0x0400

/* c_oflag bits */
#define OPOST 0x0001
#define ONLCR 0x0002

/* c_cflag bits */
#define CSIZE 0x0030
#define CS5 0x0000
#define CS6 0x0010
#define CS7 0x0020
#define CS8 0x0030
#define CREAD 0x0080
#define CLOCAL 0x0800

/* c_lflag bits */
#define ISIG 0x0001
#define ICANON 0x0002
#define ECHO 0x0008
#define ECHOE 0x0010
#define ECHOK 0x0020
#define ECHONL 0x0040
#define NOFLSH 0x0080
#define IEXTEN 0x0100
#define TOSTOP 0x0200

/* tcsetattr optional_actions */
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

/* tcflush queue_selector */
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

/* tcflow action */
#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

/* speeds */
#define B0 0
#define B50 50
#define B75 75
#define B110 110
#define B134 134
#define B150 150
#define B200 200
#define B300 300
#define B600 600
#define B1200 1200
#define B1800 1800
#define B2400 2400
#define B4800 4800
#define B9600 9600
#define B19200 19200
#define B38400 38400
#define B57600 57600
#define B115200 115200

typedef uint32_t tcflag_t;
typedef uint8_t cc_t;
typedef uint32_t speed_t;

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_cc[NCCS];
    speed_t c_ispeed;
    speed_t c_ospeed;
};

/* ioctl requests */
#define TCGETS 0x5401
#define TCSETS 0x5402
#define TCSETSW 0x5403
#define TCSETSF 0x5404
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGPTN  0x80045430  /* Get PTY slave number */
#define TIOCSCTTY 0x540E      /* Set controlling terminal */

struct winsize {
    uint16_t ws_row;
    uint16_t ws_col;
    uint16_t ws_xpixel;
    uint16_t ws_ypixel;
};

static inline void cfmakeraw(struct termios *t) {
    t->c_iflag &=
        ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    t->c_oflag &= ~OPOST;
    t->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    t->c_cflag &= ~(CSIZE);
    t->c_cflag |= CS8;
    t->c_cc[VMIN] = 1;
    t->c_cc[VTIME] = 0;
}

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);

/* Stubs for POSIX termios functions not fully implemented in xv6 */
static inline int tcflow(int fd, int action) { (void)fd; (void)action; return 0; }
static inline int tcdrain(int fd) { (void)fd; return 0; }
static inline int tcflush(int fd, int queue_selector) { (void)fd; (void)queue_selector; return 0; }
static inline speed_t cfgetospeed(const struct termios *t) { return t->c_ospeed; }
static inline int cfsetospeed(struct termios *t, speed_t s) { t->c_ospeed = s; return 0; }
static inline speed_t cfgetispeed(const struct termios *t) { return t->c_ispeed; }
static inline int cfsetispeed(struct termios *t, speed_t s) { t->c_ispeed = s; return 0; }

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TERMIOS_H */
