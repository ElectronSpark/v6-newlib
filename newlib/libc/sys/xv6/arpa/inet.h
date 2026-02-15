/**
 * @file arpa/inet.h
 * @brief Byte-order and address conversion functions for xv6/newlib
 */

#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <stdint.h>
#include <netinet/in.h>

/* Byte-order conversion (RISC-V is little-endian) */
static inline uint16_t htons(uint16_t x) {
    return (uint16_t)((x >> 8) | (x << 8));
}

static inline uint16_t ntohs(uint16_t x) {
    return htons(x);
}

static inline uint32_t htonl(uint32_t x) {
    return ((x >> 24) & 0x000000FF) |
           ((x >>  8) & 0x0000FF00) |
           ((x <<  8) & 0x00FF0000) |
           ((x << 24) & 0xFF000000);
}

static inline uint32_t ntohl(uint32_t x) {
    return htonl(x);
}

/**
 * Convert IPv4 dotted-decimal string to network byte order.
 * Minimal implementation supporting "a.b.c.d" only.
 */
static inline in_addr_t inet_addr(const char *cp) {
    unsigned int a, b, c, d;
    int i = 0, n = 0, pos = 0;
    unsigned int parts[4] = {0, 0, 0, 0};

    for (i = 0; cp[i] != '\0' && pos < 4; i++) {
        if (cp[i] >= '0' && cp[i] <= '9') {
            n = n * 10 + (cp[i] - '0');
        } else if (cp[i] == '.') {
            if (n > 255) return INADDR_NONE;
            parts[pos++] = n;
            n = 0;
        } else {
            return INADDR_NONE;
        }
    }
    if (pos != 3 || n > 255) return INADDR_NONE;
    parts[3] = n;

    a = parts[0]; b = parts[1]; c = parts[2]; d = parts[3];
    return htonl((a << 24) | (b << 16) | (c << 8) | d);
}

/**
 * Convert network byte order IPv4 address to dotted-decimal string.
 * Uses a static buffer (not thread-safe).
 */
static inline char *inet_ntoa(struct in_addr in) {
    static char buf[16];
    uint32_t addr = ntohl(in.s_addr);
    int i, pos = 0;
    unsigned int octets[4];
    octets[0] = (addr >> 24) & 0xFF;
    octets[1] = (addr >> 16) & 0xFF;
    octets[2] = (addr >>  8) & 0xFF;
    octets[3] =  addr        & 0xFF;

    for (i = 0; i < 4; i++) {
        unsigned int v = octets[i];
        if (v >= 100) buf[pos++] = '0' + v / 100;
        if (v >= 10)  buf[pos++] = '0' + (v / 10) % 10;
        buf[pos++] = '0' + v % 10;
        if (i < 3) buf[pos++] = '.';
    }
    buf[pos] = '\0';
    return buf;
}

#endif /* _ARPA_INET_H */
