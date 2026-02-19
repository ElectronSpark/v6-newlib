/*
 * xv6 override for <sys/wait.h>
 *
 * Chains to newlib's sys/wait.h via #include_next, then adds BSD
 * extensions (wait3) that dash and other programs expect.
 */
#ifndef _XV6_SYS_WAIT_H
#define _XV6_SYS_WAIT_H

#include_next <sys/wait.h>

#include <sys/types.h>

struct rusage;  /* forward declaration from <sys/resource.h> */

pid_t wait3(int *status, int options, struct rusage *rusage);

#endif /* _XV6_SYS_WAIT_H */
