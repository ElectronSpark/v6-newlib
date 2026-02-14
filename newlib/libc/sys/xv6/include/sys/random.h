/*
 * sys/random.h - Random number generation
 * xv6 compatibility header for CPython
 */
#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#include <sys/types.h>

/* Flags for getrandom() */
#define GRND_NONBLOCK   0x0001
#define GRND_RANDOM     0x0002
#define GRND_INSECURE   0x0004

#ifdef __cplusplus
extern "C" {
#endif

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);
int getentropy(void *buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_RANDOM_H */
