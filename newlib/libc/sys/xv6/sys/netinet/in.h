/**
 * @file netinet/in.h
 * @brief Internet address family definitions for xv6/newlib
 *
 * Provides struct sockaddr_in, struct in_addr, INADDR_* constants,
 * and byte-order conversion functions for IPv4 networking.
 */

#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>

/* --- Byte-order conversion (RISC-V is little-endian) --- */

static inline uint16_t __xv6_bswap16(uint16_t x) {
    return __builtin_bswap16(x);
}
static inline uint32_t __xv6_bswap32(uint32_t x) {
    return __builtin_bswap32(x);
}

#define htons(x) __xv6_bswap16(x)
#define ntohs(x) __xv6_bswap16(x)
#define htonl(x) __xv6_bswap32(x)
#define ntohl(x) __xv6_bswap32(x)

/* --- Types --- */

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

/* --- Internet address --- */

struct in_addr {
    in_addr_t s_addr;
};

/* --- IPv4 socket address --- */

struct sockaddr_in {
    sa_family_t    sin_family;   /* AF_INET */
    in_port_t      sin_port;     /* port in network byte order */
    struct in_addr sin_addr;     /* internet address */
    char           sin_zero[8];  /* padding to sizeof(struct sockaddr) */
};

/* --- IPv6 (stub — enough for CPython to compile) --- */

struct in6_addr {
    uint8_t s6_addr[16];
};

struct sockaddr_in6 {
    sa_family_t     sin6_family;   /* AF_INET6 */
    in_port_t       sin6_port;     /* port in network byte order */
    uint32_t        sin6_flowinfo; /* IPv6 flow information */
    struct in6_addr sin6_addr;     /* IPv6 address */
    uint32_t        sin6_scope_id; /* Scope ID */
};

/* Wildcard / broadcast / loopback */
#define INADDR_ANY       ((in_addr_t) 0x00000000)
#define INADDR_BROADCAST ((in_addr_t) 0xffffffff)
#define INADDR_LOOPBACK  ((in_addr_t) 0x7f000001)
#define INADDR_NONE      ((in_addr_t) 0xffffffff)

#define INET_ADDRSTRLEN   16
#define INET6_ADDRSTRLEN  46

/* IP protocol numbers (supplement sys/socket.h) */
#ifndef IPPROTO_IP
#define IPPROTO_IP   0
#endif
#ifndef IPPROTO_ICMP
#define IPPROTO_ICMP 1
#endif
#ifndef IPPROTO_TCP
#define IPPROTO_TCP  6
#endif
#ifndef IPPROTO_UDP
#define IPPROTO_UDP  17
#endif
#ifndef IPPROTO_IPV6
#define IPPROTO_IPV6 41
#endif
#ifndef IPPROTO_RAW
#define IPPROTO_RAW  255
#endif

/* IPv6 socket options */
#define IPV6_JOIN_GROUP    20
#define IPV6_LEAVE_GROUP   21
#define IPV6_MULTICAST_HOPS 18
#define IPV6_MULTICAST_IF   17
#define IPV6_MULTICAST_LOOP 19
#define IPV6_UNICAST_HOPS   16
#define IPV6_V6ONLY         26

/* IP-level socket options */
#define IP_TOS             1
#define IP_TTL             2
#define IP_ADD_MEMBERSHIP  35
#define IP_DROP_MEMBERSHIP 36
#define IP_MULTICAST_IF    32
#define IP_MULTICAST_TTL   33
#define IP_MULTICAST_LOOP  34

/* Multicast group request */
struct ip_mreq {
    struct in_addr imr_multiaddr;
    struct in_addr imr_interface;
};

#endif /* _NETINET_IN_H */
