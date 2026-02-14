/*
 * xv6_time.h - Time extensions for xv6
 * 
 * Provides POSIX time function declarations and constants
 * for programs that need clock_gettime() etc.
 */

#ifndef _XV6_TIME_H
#define _XV6_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Don't include sys/types.h here - let the including file do that */

/* Clock IDs for clock_gettime/clock_getres */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME          0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC         1
#endif

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW     4
#endif

#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID 2
#endif

#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID 3
#endif

/* Timer flags */
#ifndef TIMER_ABSTIME
#define TIMER_ABSTIME           1
#endif

/* Forward declarations - actual definitions in newlib_syscalls.c */
struct timespec;

int clock_gettime(int clk_id, struct timespec *tp);
int clock_getres(int clk_id, struct timespec *res);
int clock_settime(int clk_id, const struct timespec *tp);
int nanosleep(const struct timespec *req, struct timespec *rem);

#ifdef __cplusplus
}
#endif

#endif /* _XV6_TIME_H */
