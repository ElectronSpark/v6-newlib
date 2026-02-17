/**
 * @file arpa/inet.h
 * @brief Internet address manipulation functions for xv6/newlib
 */

#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <netinet/in.h>

/* Provided by newlib_syscalls.c */
in_addr_t   inet_addr(const char *cp);
char        *inet_ntoa(struct in_addr in);
int          inet_aton(const char *cp, struct in_addr *inp);
int          inet_pton(int af, const char *src, void *dst);
const char  *inet_ntop(int af, const void *src, char *dst, socklen_t size);

#endif /* _ARPA_INET_H */
