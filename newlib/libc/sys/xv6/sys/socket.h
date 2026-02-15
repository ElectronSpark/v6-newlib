/**
 * @file sys/socket.h
 * @brief POSIX socket API definitions for xv6/newlib
 *
 * Provides the minimal set of socket types, constants, and function
 * declarations needed for user-space networking over xv6's lwIP-backed
 * socket syscalls.
 */

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <sys/types.h>
#include <stdint.h>

/* Address families */
#define AF_UNSPEC      0
#define AF_INET        2
#define AF_INET6       10

/* Protocol families (same as AF_*) */
#define PF_UNSPEC      AF_UNSPEC
#define PF_INET        AF_INET
#define PF_INET6       AF_INET6

/* Socket types */
#define SOCK_STREAM    1
#define SOCK_DGRAM     2
#define SOCK_RAW       3

/* Protocols */
#define IPPROTO_IP     0
#define IPPROTO_ICMP   1
#define IPPROTO_TCP    6
#define IPPROTO_UDP    17

/* shutdown() how */
#define SHUT_RD        0
#define SHUT_WR        1
#define SHUT_RDWR      2

/* Socket option levels */
#define SOL_SOCKET     1

/* Socket options */
#define SO_REUSEADDR   2
#define SO_ERROR       4
#define SO_SNDBUF      7
#define SO_RCVBUF      8
#define SO_KEEPALIVE   9
#define SO_RCVTIMEO    20
#define SO_SNDTIMEO    21

/* Message flags (for send/recv) */
#define MSG_PEEK       0x02
#define MSG_DONTWAIT   0x40
#define MSG_NOSIGNAL   0x4000

/* Generic socket address (for type compatibility) */
typedef unsigned short sa_family_t;
typedef uint32_t socklen_t;

struct sockaddr {
    sa_family_t sa_family;
    char        sa_data[14];
};

/* Storage large enough for any address type */
struct sockaddr_storage {
    sa_family_t ss_family;
    char        __ss_pad[126];
};

/* Function declarations — implemented by newlib syscall stubs */
int socket(int domain, int type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
int getsockopt(int sockfd, int level, int optname,
               void *optval, socklen_t *optlen);
int shutdown(int sockfd, int how);
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

#endif /* _SYS_SOCKET_H */
