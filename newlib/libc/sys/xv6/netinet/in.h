/**
 * @file netinet/in.h
 * @brief IPv4 address structures and constants for xv6/newlib
 */

#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <sys/socket.h>
#include <stdint.h>

/* in_port_t and in_addr_t */
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

/* IPv4 address */
struct in_addr {
    in_addr_t s_addr;  /* network byte order */
};

/* IPv4 socket address */
struct sockaddr_in {
    sa_family_t    sin_family;   /* AF_INET */
    in_port_t      sin_port;     /* port in network byte order */
    struct in_addr sin_addr;     /* IPv4 address */
    char           sin_zero[8];  /* padding to match sizeof(struct sockaddr) */
};

/* Special addresses */
#define INADDR_ANY       ((in_addr_t)0x00000000)
#define INADDR_BROADCAST ((in_addr_t)0xFFFFFFFF)
#define INADDR_LOOPBACK  ((in_addr_t)0x7F000001)  /* 127.0.0.1 host order */
#define INADDR_NONE      ((in_addr_t)0xFFFFFFFF)

/* Ports */
#define IPPORT_RESERVED  1024

#endif /* _NETINET_IN_H */
