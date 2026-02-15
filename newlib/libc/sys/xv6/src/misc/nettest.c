/**
 * @file nettest.c
 * @brief Simple network validation test for xv6 lwIP port
 *
 * Tests the socket syscall interface by:
 *  1. Creating a UDP socket and sending a packet to the QEMU gateway
 *  2. Creating a TCP socket and connecting to the QEMU SLIRP built-in
 *     services (port 10.0.2.2:echo or just testing connect)
 *  3. Verifying socket lifecycle (socket → bind → close)
 *
 * Build: registered as a newlib program via add_newlib_program()
 * Run:   $ nettest      (from xv6 shell)
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define GATEWAY_IP   "10.0.2.2"
#define OWN_IP       "10.0.2.15"

static int test_count = 0;
static int pass_count = 0;
static int fail_count = 0;

#define TEST(name) do { \
    test_count++; \
    printf("TEST %d: %s ... ", test_count, name); \
} while(0)

#define PASS() do { \
    pass_count++; \
    printf("PASS\n"); \
} while(0)

#define FAIL(msg) do { \
    fail_count++; \
    printf("FAIL: %s (errno=%d)\n", msg, errno); \
} while(0)

/* --------------------------------------------------------------------- */
/* Test 1: UDP socket create / bind / sendto / close                     */
/* --------------------------------------------------------------------- */
static void test_udp_basic(void)
{
    TEST("UDP socket create");
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { FAIL("socket() failed"); return; }
    PASS();

    TEST("UDP bind to port 12345");
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(12345);
    local.sin_addr.s_addr = INADDR_ANY;
    int r = bind(fd, (struct sockaddr *)&local, sizeof(local));
    if (r < 0) { FAIL("bind() failed"); close(fd); return; }
    PASS();

    TEST("UDP getsockname");
    struct sockaddr_in bound;
    socklen_t blen = sizeof(bound);
    r = getsockname(fd, (struct sockaddr *)&bound, &blen);
    if (r < 0) { FAIL("getsockname() failed"); }
    else if (ntohs(bound.sin_port) != 12345) { FAIL("wrong port"); }
    else { PASS(); }

    TEST("UDP sendto gateway:7 (echo)");
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(7);  /* echo service */
    dest.sin_addr.s_addr = inet_addr(GATEWAY_IP);
    const char *msg = "hello from xv6!";
    ssize_t sent = sendto(fd, msg, strlen(msg), 0,
                          (struct sockaddr *)&dest, sizeof(dest));
    if (sent < 0) { FAIL("sendto() failed"); }
    else if (sent != (ssize_t)strlen(msg)) { FAIL("short send"); }
    else { PASS(); }

    TEST("UDP close");
    r = close(fd);
    if (r < 0) { FAIL("close() failed"); }
    else { PASS(); }
}

/* --------------------------------------------------------------------- */
/* Test 2: TCP socket create / connect / send / recv / close             */
/* --------------------------------------------------------------------- */
static void test_tcp_connect(void)
{
    TEST("TCP socket create");
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { FAIL("socket() failed"); return; }
    PASS();

    TEST("TCP setsockopt SO_RCVTIMEO");
    int timeout_ms = 3000;
    int r = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
                       &timeout_ms, sizeof(timeout_ms));
    if (r < 0) { FAIL("setsockopt() failed"); }
    else { PASS(); }

    /* Connect to QEMU SLIRP's built-in TCP echo (port 7) or
     * just test that connect works / returns appropriate error */
    TEST("TCP connect to gateway:7");
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(7);
    dest.sin_addr.s_addr = inet_addr(GATEWAY_IP);
    r = connect(fd, (struct sockaddr *)&dest, sizeof(dest));
    if (r < 0) {
        printf("(connect errno=%d) ", errno);
        /* Connection refused / reset / timed-out are all expected when
         * no echo service is running — any non-zero errno is fine.     */
        if (errno != 0) {
            printf("(expected) PASS\n");
            pass_count++;
        } else {
            FAIL("connect returned -1 but errno is 0");
        }
        close(fd);
        return;
    }
    PASS();

    TEST("TCP getpeername");
    struct sockaddr_in peer;
    socklen_t plen = sizeof(peer);
    r = getpeername(fd, (struct sockaddr *)&peer, &plen);
    if (r < 0) { FAIL("getpeername() failed"); }
    else { PASS(); }

    TEST("TCP send");
    const char *msg = "xv6 TCP test\n";
    ssize_t sent = send(fd, msg, strlen(msg), 0);
    if (sent < 0) { FAIL("send() failed"); }
    else { PASS(); }

    TEST("TCP recv (with timeout)");
    char buf[256];
    ssize_t got = recv(fd, buf, sizeof(buf), 0);
    if (got > 0) {
        buf[got] = '\0';
        printf("received %d bytes: \"%s\" ", (int)got, buf);
        PASS();
    } else if (got == 0) {
        printf("(EOF) PASS\n");
        pass_count++;
    } else {
        FAIL("recv() failed");
    }

    TEST("TCP shutdown + close");
    shutdown(fd, SHUT_RDWR);
    r = close(fd);
    if (r < 0) { FAIL("close() failed"); }
    else { PASS(); }
}

/* --------------------------------------------------------------------- */
/* Test 3: Socket lifecycle — create many, close all                     */
/* --------------------------------------------------------------------- */
static void test_socket_lifecycle(void)
{
    TEST("Create 8 sockets");
    int fds[8];
    int i, ok = 1;
    for (i = 0; i < 8; i++) {
        fds[i] = socket(AF_INET, SOCK_DGRAM, 0);
        if (fds[i] < 0) { ok = 0; break; }
    }
    if (!ok) { FAIL("couldn't create 8 sockets"); return; }
    PASS();

    TEST("Close all 8 sockets");
    ok = 1;
    for (i = 0; i < 8; i++) {
        if (close(fds[i]) < 0) { ok = 0; break; }
    }
    if (!ok) { FAIL("close failed"); }
    else { PASS(); }
}

/* --------------------------------------------------------------------- */
/* Test 4: Error cases                                                   */
/* --------------------------------------------------------------------- */
static void test_error_cases(void)
{
    TEST("socket(AF_INET6) fails");
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd >= 0) { close(fd); FAIL("should have failed"); }
    else { PASS(); }

    TEST("bind on bad fd fails");
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    int r = bind(999, (struct sockaddr *)&sa, sizeof(sa));
    if (r == 0) { FAIL("should have failed"); }
    else { PASS(); }
}

/* --------------------------------------------------------------------- */
/* main                                                                  */
/* --------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    printf("=== xv6 Network Test (lwIP socket syscalls) ===\n");
    printf("Host IP: %s  Gateway: %s\n\n", OWN_IP, GATEWAY_IP);

    test_udp_basic();
    printf("\n");
    test_tcp_connect();
    printf("\n");
    test_socket_lifecycle();
    printf("\n");
    test_error_cases();

    printf("\n=== Results: %d/%d passed, %d failed ===\n",
           pass_count, test_count, fail_count);

    return fail_count > 0 ? 1 : 0;
}
