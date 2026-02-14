/*
 * xv6_cpython.h - CPython-specific extensions for xv6/newlib
 * 
 * This file is included via -include flag before any other headers.
 * It should ONLY provide #defines and extern declarations.
 * DO NOT typedef anything here as it will conflict with newlib.
 */

#ifndef _XV6_CPYTHON_H
#define _XV6_CPYTHON_H

/* Ensure BSD extensions are visible - must be set before any system headers */
#ifndef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#endif

/* BSD types needed when __BSD_VISIBLE is set */
#ifndef _BSD_TYPES_DEFINED
#define _BSD_TYPES_DEFINED
typedef unsigned int u_int;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Clock IDs - define these before time.h is included */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME          0
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC         1
#endif
#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW     4
#endif

/* Signal flags - define before signal.h is included */
#ifndef SA_ONSTACK
#define SA_ONSTACK      0x08000000
#endif
#ifndef SA_RESTART
#define SA_RESTART      0x10000000
#endif
#ifndef SA_NODEFER
#define SA_NODEFER      0x40000000
#endif
#ifndef SA_RESETHAND
#define SA_RESETHAND    0x80000000
#endif

/* POSIX thread priority scheduling - required to enable pthread_attr_setscope
 * and related declarations in newlib's pthread.h */
#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#define _POSIX_THREAD_PRIORITY_SCHEDULING 1
#endif

/* Pthread scope constants - needed by CPython */
#ifndef PTHREAD_SCOPE_SYSTEM
#define PTHREAD_SCOPE_SYSTEM    0
#endif
#ifndef PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_PROCESS   1
#endif

/*
 * Function declarations - use generic types to avoid conflicts.
 * These forward declare functions that xv6 provides but may not be
 * declared by newlib in all configurations.
 */

/* Time functions - newlib will provide struct timespec */
int clock_gettime(int clk_id, void *tp);
int clock_getres(int clk_id, void *res);
int nanosleep(const void *req, void *rem);

/*
 * Signal functions - newlib's signal.h already declares these with proper
 * sigset_t types. We just need to ensure the implementations exist in
 * newlib_syscalls.c
 */

/* memrchr - xv6 provides this */
void *memrchr(const void *s, int c, unsigned long n);

/*
 * Locale support - xv6 doesn't have full locale support.
 * nl_langinfo() is used by CPython to determine encoding.
 * We provide a minimal stub that returns UTF-8.
 */
#ifndef CODESET
#define CODESET         0
#endif

/* nl_langinfo stub - always returns UTF-8 */
static inline char *nl_langinfo(int __item) {
    (void)__item;
    return "UTF-8";
}

/*
 * Timezone support - xv6 uses UTC, no daylight saving time
 * Note: timezone is a variable, not a macro, to avoid conflicts
 */
#ifndef daylight
#define daylight 0
#endif

/* Declare timezone as an extern long - we'll define it in newlib_syscalls.c */
extern long timezone;

/* utime - set file access and modification times
 * Old-style interface that takes time_t[2] instead of struct utimbuf 
 */
int utime(const char *path, const long *times);

/*
 * lstat - CPython uses this for symlink-aware stat
 * We can alias it to stat since xv6 doesn't have symlinks
 */
struct stat;
int lstat(const char *path, struct stat *buf);

/*
 * Directory functions - CPython needs opendir/readdir/closedir
 * Now that HAVE_DIRENT_H is set, CPython will include dirent.h directly.
 * We don't forward-declare these here to avoid conflicts.
 */

/*
 * fileno and fdopen - POSIX functions for file descriptors
 * CPython needs these for tokenizer and readline.
 * We forward-declare with opaque FILE type.
 */
struct __sFILE;
typedef struct __sFILE FILE;
int fileno(FILE *stream);
FILE *fdopen(int fd, const char *mode);

/*
 * Pthread functions are declared by newlib's pthread.h - we just need
 * to provide the implementations in pthread.c
 */

#ifdef __cplusplus
}
#endif

#endif /* _XV6_CPYTHON_H */
