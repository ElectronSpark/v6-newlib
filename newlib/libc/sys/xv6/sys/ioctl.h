/*
 * sys/ioctl.h - xv6 ioctl interface for newlib
 */
#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <sys/types.h>
#include <sys/termios.h>

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int fd, unsigned long request, ...);

/* Common query for bytes available to read (Linux-compatible value). */
#ifndef FIONREAD
#define FIONREAD 0x541B
#endif

#ifndef TCFLSH
#define TCFLSH 0x540B
#endif

#ifndef TIOCFLUSH
#define TIOCFLUSH TCFLSH
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IOCTL_H */
