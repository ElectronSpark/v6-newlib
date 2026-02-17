/**
 * @file netinet/tcp.h
 * @brief TCP protocol definitions for xv6/newlib
 */

#ifndef _NETINET_TCP_H
#define _NETINET_TCP_H

/* TCP socket options (level IPPROTO_TCP) */
#define TCP_NODELAY    1   /* Don't delay send to coalesce packets */
#define TCP_KEEPIDLE   4   /* Start keeplives after this period */
#define TCP_KEEPINTVL  5   /* Interval between keepalives */
#define TCP_KEEPCNT    6   /* Number of keepalives before death */

#endif /* _NETINET_TCP_H */
