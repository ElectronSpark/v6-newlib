/**
 * @file newlib_syscalls.c
 * @brief Newlib system call stubs for xv6
 *
 * This file provides the low-level system call stubs that newlib's libc
 * requires. These stubs translate between newlib's expected interface
 * (using _underscore prefixed names) and xv6's actual system calls.
 *
 * Required by newlib's libc.a for stdio, malloc, process control, etc.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/times.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

/* Declare memcpy/memset/strlen/strchr/strncpy directly to avoid kernel/newlib string.h conflicts */
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern size_t strlen(const char *s);
extern char *strchr(const char *s, int c);
extern char *strncpy(char *dest, const char *src, size_t n);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);

/* errno - required by newlib.
 * We define the global symbol for backward compatibility, then
 * redirect all subsequent uses through newlib's reentrant __errno()
 * so that errno writes are visible to code (e.g. CPython) that reads
 * errno via (*__errno()) / _impure_ptr->_errno.
 */
#undef errno
int errno;
extern int *__errno(void);
#define errno (*__errno())

/*
 * Timezone variables for POSIX compatibility
 * xv6 uses UTC, no timezone offset or daylight saving time
 */
long timezone = 0;
int daylight = 0;

/* 
 * xv6 syscall wrappers - these are assembly stubs from usys.S
 * We use different names to avoid conflicts with newlib declarations
 */
extern int _xv6_read(int fd, void *buf, int n) __asm__("read");
extern int _xv6_write(int fd, const void *buf, int n) __asm__("write");
extern int _xv6_open(const char *path, int flags) __asm__("open");
extern int _xv6_close(int fd) __asm__("close");
extern int _xv6_fork(void) __asm__("fork");
extern int _xv6_exit(int status) __asm__("exit");
extern int _xv6_wait(int *status) __asm__("wait");
extern int _xv6_kill(int pid, int sig) __asm__("kill");
extern int _xv6_getpid(void) __asm__("getpid");
extern int _xv6_exec(const char *path, char **argv, char **envp) __asm__("exec");
extern int _xv6_pipe(int *fds) __asm__("pipe");
extern int _xv6_dup(int fd) __asm__("dup");
extern int _xv6_dup2(int oldfd, int newfd) __asm__("dup2");
extern int _xv6_link(const char *old, const char *new) __asm__("link");
extern int _xv6_unlink(const char *path) __asm__("unlink");
extern int _xv6_symlink(const char *target, const char *linkpath) __asm__("symlink");
extern int _xv6_mkdir(const char *path) __asm__("mkdir");
extern int _xv6_chdir(const char *path) __asm__("chdir");
extern char *_xv6_sbrk(int64_t n) __asm__("sbrk");
extern int64_t _xv6_lseek(int fd, int64_t offset, int whence) __asm__("lseek");
extern int _xv6_gettimeofday(struct timeval *tv, void *tz) __asm__("gettimeofday");
extern int _xv6_getcwd(char *buf, int size) __asm__("getcwd");
extern int _xv6_fcntl(int fd, int cmd, int arg) __asm__("fcntl");
extern int _xv6_access(const char *path, int mode) __asm__("access");
extern int _xv6_waitpid(int pid, int *status, int options) __asm__("waitpid");
extern int _xv6_nanosleep(const void *req, void *rem) __asm__("nanosleep");
extern int _xv6_ftruncate(int fd, int64_t length) __asm__("ftruncate");
extern int _xv6_rename(const char *oldpath, const char *newpath) __asm__("rename");
extern int _xv6_getppid(void) __asm__("getppid");
extern int _xv6_uname(void *buf) __asm__("uname");
extern uint64_t _xv6_uptime(void) __asm__("uptime");
extern int _xv6_ioctl(int fd, int request, uint64_t arg) __asm__("ioctl");
extern int _xv6_poll(void *fds, int nfds, int timeout) __asm__("__xv6_poll");

/* Network / socket syscalls (assembly stubs in usys.S, prefixed __xv6_*) */
extern int _xv6_socket(int domain, int type, int protocol) __asm__("__xv6_socket");
extern int _xv6_bind(int fd, const void *addr, int addrlen) __asm__("__xv6_bind");
extern int _xv6_listen(int fd, int backlog) __asm__("__xv6_listen");
extern int _xv6_accept(int fd, void *addr, void *addrlen) __asm__("__xv6_accept");
extern int _xv6_sconnect(int fd, const void *addr, int addrlen) __asm__("__xv6_sconnect");
extern int _xv6_sendto(int fd, const void *buf, int len, int flags,
                        const void *dest_addr, int addrlen) __asm__("__xv6_sendto");
extern int _xv6_recvfrom(int fd, void *buf, int len, int flags,
                          void *src_addr, void *addrlen) __asm__("__xv6_recvfrom");
extern int _xv6_setsockopt(int fd, int level, int optname,
                            const void *optval, int optlen) __asm__("__xv6_setsockopt");
extern int _xv6_getsockopt(int fd, int level, int optname,
                            void *optval, void *optlen) __asm__("__xv6_getsockopt");
extern int _xv6_shutdown(int fd, int how) __asm__("__xv6_shutdown");
extern int _xv6_getpeername(int fd, void *addr, void *addrlen) __asm__("__xv6_getpeername");
extern int _xv6_getsockname(int fd, void *addr, void *addrlen) __asm__("__xv6_getsockname");

/* getrandom is provided by usys.S syscall stub */
extern ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);

/* mmap/munmap syscalls */
extern void *_xv6_mmap(void *addr, uint64_t length, int prot, int flags, int fd, uint64_t offset) __asm__("mmap");
extern int _xv6_munmap(void *addr, uint64_t length) __asm__("munmap");
extern int _xv6_mprotect(void *addr, uint64_t length, int prot) __asm__("mprotect");

/* Signal syscalls */
struct xv6_sigaction {
    union {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, void *, void *);
    };
    uint64_t sa_mask;
    int sa_flags;
};
extern int _xv6_sigaction(int signum, struct xv6_sigaction *act, struct xv6_sigaction *oldact) __asm__("sigaction");
extern int _xv6_sigprocmask(int how, const uint64_t *set, uint64_t *oldset) __asm__("sigprocmask");
extern int _xv6_kill(int pid, int sig) __asm__("kill");

/*
 * stat/fstat/lstat syscalls
 *
 * xv6 kernel uses a compact stat layout that differs from newlib's
 * struct stat ABI. Convert fields explicitly in userspace wrappers.
 */

struct xv6_kstat {
    int32_t dev;
    uint64_t ino;
    uint32_t mode;
    uint32_t nlink;
    uint64_t size;
};

struct stat64;

extern int _xv6_stat(const char *path, void *st) __asm__("__xv6_stat");
extern int _xv6_lstat(const char *path, void *st) __asm__("__xv6_lstat");
extern int _xv6_fstat(int fd, void *st) __asm__("__xv6_fstat");
extern ssize_t _xv6_readlink(const char *path, char *buf, int bufsiz) __asm__("readlink");

static int xv6_kstat_to_newlib(const struct xv6_kstat *kst,
                               struct stat *nst) {
    memset(nst, 0, sizeof(*nst));
    {
        uint64_t dev = (uint32_t)kst->dev;
        nst->st_dev = (dev_t)dev;
    }
    {
        uint64_t ino = kst->ino;
        nst->st_ino = (ino_t)ino;
    }
    nst->st_mode = (mode_t)kst->mode;
    {
        uint64_t nlink = kst->nlink;
        nst->st_nlink = (nlink_t)nlink;
    }
    {
        uint64_t size = kst->size;
        off_t out = (off_t)size;
        if ((uint64_t)out != size) {
            errno = EOVERFLOW;
            return -1;
        }
        nst->st_size = out;
    }
    return 0;
}

/*
 * Flag translation: Newlib uses different values than xv6/musl
 * 
 * Newlib flags (from sys/_default_fcntl.h):
 *   O_RDONLY  = 0x0000
 *   O_WRONLY  = 0x0001
 *   O_RDWR    = 0x0002
 *   O_APPEND  = 0x0008
 *   O_CREAT   = 0x0200
 *   O_TRUNC   = 0x0400
 *   O_EXCL    = 0x0800
 *
 * xv6/musl flags (from kernel/inc/vfs/fcntl.h):
 *   O_RDONLY  = 00
 *   O_WRONLY  = 01
 *   O_RDWR    = 02
 *   O_CREAT   = 0100
 *   O_EXCL    = 0200
 *   O_TRUNC   = 01000
 *   O_APPEND  = 02000
 */

/* Newlib flag values */
#define NEWLIB_O_RDONLY   0x0000
#define NEWLIB_O_WRONLY   0x0001
#define NEWLIB_O_RDWR     0x0002
#define NEWLIB_O_APPEND   0x0008
#define NEWLIB_O_CREAT    0x0200
#define NEWLIB_O_TRUNC    0x0400
#define NEWLIB_O_EXCL     0x0800

/* xv6/musl flag values */
#define XV6_O_RDONLY      00
#define XV6_O_WRONLY      01
#define XV6_O_RDWR        02
#define XV6_O_CREAT       0100
#define XV6_O_EXCL        0200
#define XV6_O_TRUNC       01000
#define XV6_O_APPEND      02000

/**
 * Translate newlib open flags to xv6/musl flags
 */
static int translate_open_flags(int newlib_flags) {
    int xv6_flags = 0;
    
    /* Access mode (mutually exclusive) */
    int accmode = newlib_flags & (NEWLIB_O_RDONLY | NEWLIB_O_WRONLY | NEWLIB_O_RDWR);
    if (accmode == NEWLIB_O_RDONLY)
        xv6_flags |= XV6_O_RDONLY;
    else if (accmode == NEWLIB_O_WRONLY)
        xv6_flags |= XV6_O_WRONLY;
    else if (accmode == NEWLIB_O_RDWR)
        xv6_flags |= XV6_O_RDWR;
    
    /* Other flags */
    if (newlib_flags & NEWLIB_O_CREAT)
        xv6_flags |= XV6_O_CREAT;
    if (newlib_flags & NEWLIB_O_TRUNC)
        xv6_flags |= XV6_O_TRUNC;
    if (newlib_flags & NEWLIB_O_APPEND)
        xv6_flags |= XV6_O_APPEND;
    if (newlib_flags & NEWLIB_O_EXCL)
        xv6_flags |= XV6_O_EXCL;
    
    return xv6_flags;
}

/**
 * Set errno and return -1 on error
 */
static inline int set_errno(int err) {
    errno = err;
    return -1;
}

/*
 * Process control
 */

void _exit(int status) {
    _xv6_exit(status);
    __builtin_unreachable();
}

int _fork(void) {
    return _xv6_fork();
}

int _execve(const char *name, char *const argv[], char *const env[]) {
    return _xv6_exec(name, (char **)argv, (char **)env);
}

int _wait(int *status) {
    return _xv6_wait(status);
}

int _kill(int pid, int sig) {
    return _xv6_kill(pid, sig);
}

int _getpid(void) {
    return _xv6_getpid();
}

/*
 * File operations
 */

int _open(const char *name, int flags, int mode) {
    (void)mode;  /* xv6 doesn't use mode in the same way */
    int xv6_flags = translate_open_flags(flags);
    int ret = _xv6_open(name, xv6_flags);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}

int _open64(const char *name, int flags, ...) {
    int mode = 0;
    if (flags & NEWLIB_O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, int);
        va_end(ap);
    }
    return _open(name, flags, mode);
}

int _close(int fd) {
    int ret = _xv6_close(fd);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}

ssize_t _read(int fd, void *buf, size_t count) {
    ssize_t ret = _xv6_read(fd, buf, (int)count);
    if (ret < 0) {
        errno = (int)(-ret);
        return -1;
    }
    return ret;
}

ssize_t _write(int fd, const void *buf, size_t count) {
    ssize_t ret = _xv6_write(fd, buf, (int)count);
    if (ret < 0) {
        errno = (int)(-ret);
        return -1;
    }
    return ret;
}

off_t _lseek(int fd, off_t offset, int whence) {
    int64_t raw = _xv6_lseek(fd, (int64_t)offset, whence);
    if (raw < 0) {
        errno = (int)(-raw);
        return -1;
    }
    off_t ret = (off_t)raw;
    if ((int64_t)ret != raw) {
        errno = EOVERFLOW;
        return -1;
    }
    return ret;
}

_off64_t _lseek64(int fd, _off64_t offset, int whence) {
    if (offset > (_off64_t)INT64_MAX || offset < (_off64_t)INT64_MIN) {
        errno = EOVERFLOW;
        return (_off64_t)-1;
    }
    int64_t raw = _xv6_lseek(fd, (int64_t)offset, whence);
    if (raw < 0) {
        errno = (int)(-raw);
        return (_off64_t)-1;
    }
    _off64_t ret = (_off64_t)raw;
    if ((int64_t)ret != raw) {
        errno = EOVERFLOW;
        return (_off64_t)-1;
    }
    return ret;
}

int _fstat(int fd, struct stat *st) {
    struct xv6_kstat kst;
    int ret = _xv6_fstat(fd, &kst);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    if (xv6_kstat_to_newlib(&kst, st) < 0) {
        return -1;
    }
    return 0;
}

int _fstat64(int fd, struct stat64 *st) {
    return _fstat(fd, (struct stat *)(void *)st);
}

int _stat(const char *path, struct stat *st) {
    struct xv6_kstat kst;
    int ret = _xv6_stat(path, &kst);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    if (xv6_kstat_to_newlib(&kst, st) < 0) {
        return -1;
    }
    return 0;
}

int _stat64(const char *path, struct stat64 *st) {
    return _stat(path, (struct stat *)(void *)st);
}

int _lstat(const char *path, struct stat *st) {
    struct xv6_kstat kst;
    int ret = _xv6_lstat(path, &kst);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    if (xv6_kstat_to_newlib(&kst, st) < 0) {
        return -1;
    }
    return 0;
}

int _lstat64(const char *path, struct stat64 *st) {
    return _lstat(path, (struct stat *)(void *)st);
}

int lstat(const char *path, struct stat *st) {
    return _lstat(path, st);
}

/* Public lstat() is routed through _lstat() conversion above. */

int _link(const char *old, const char *new) {
    return _xv6_link(old, new);
}

int _unlink(const char *name) {
    return _xv6_unlink(name);
}

int _dup(int fd) {
    return _xv6_dup(fd);
}

int _dup2(int oldfd, int newfd) {
    return _xv6_dup2(oldfd, newfd);
}

int _pipe(int pipefd[2]) {
    return _xv6_pipe(pipefd);
}

int _mkdir(const char *path, mode_t mode) {
    (void)mode;  /* xv6 doesn't use mode in mkdir */
    return _xv6_mkdir(path);
}

int _chdir(const char *path) {
    return _xv6_chdir(path);
}

int _rmdir(const char *path) {
    /* xv6 uses unlink for directories too */
    return _xv6_unlink(path);
}

int _symlink(const char *target, const char *linkpath) {
    return _xv6_symlink(target, linkpath);
}

int _readlink(const char *path, char *buf, size_t bufsize) {
    ssize_t ret = _xv6_readlink(path, buf, (int)bufsize);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return (int)ret;
}

int _isatty(int fd) {
    struct xv6_kstat kst;
    int ret = _xv6_fstat(fd, &kst);
    if (ret < 0) {
        errno = -ret;
        return 0;
    }

    if (S_ISCHR(kst.mode)) {
        return 1;
    }

    errno = ENOTTY;
    return 0;
}

char *_getcwd(char *buf, size_t size) {
    if (_xv6_getcwd(buf, (int)size) < 0) {
        errno = ERANGE;
        return NULL;
    }
    return buf;
}

/*
 * Memory allocation
 */

void *_sbrk(ptrdiff_t incr) {
    char *prev_heap_end = _xv6_sbrk((int64_t)incr);
    if (prev_heap_end == (char *)-1) {
        errno = ENOMEM;
        return (void *)-1;
    }
    return prev_heap_end;
}

/*
 * Time functions
 */

int _gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;  /* xv6 doesn't support timezone */
    return _xv6_gettimeofday(tv, tz);
}

clock_t _times(struct tms *buf) {
    /* xv6 doesn't have times syscall - return minimal info */
    if (buf) {
        buf->tms_utime = 0;
        buf->tms_stime = 0;
        buf->tms_cutime = 0;
        buf->tms_cstime = 0;
    }
    return 0;
}

/*
 * clock_gettime - POSIX high-resolution clock
 * 
 * Supports:
 *   CLOCK_REALTIME  (1) - Wall clock time (uses gettimeofday)
 *   CLOCK_MONOTONIC (4) - Monotonic time since boot (uses uptime)
 * 
 * Note: These values match newlib's definitions in time.h
 */
#define XV6_CLOCK_REALTIME_COARSE    0
#define XV6_CLOCK_REALTIME           1
#define XV6_CLOCK_PROCESS_CPUTIME_ID 2
#define XV6_CLOCK_THREAD_CPUTIME_ID  3
#define XV6_CLOCK_MONOTONIC          4
#define XV6_CLOCK_MONOTONIC_RAW      5
#define XV6_CLOCK_MONOTONIC_COARSE   6

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    if (tp == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    switch (clk_id) {
    case XV6_CLOCK_REALTIME_COARSE:
    case XV6_CLOCK_REALTIME: {
        /* Use gettimeofday for wall clock time */
        struct timeval tv;
        int ret = _xv6_gettimeofday(&tv, NULL);
        if (ret < 0) {
            errno = -ret;
            return -1;
        }
        tp->tv_sec = tv.tv_sec;
        tp->tv_nsec = tv.tv_usec * 1000;  /* usec to nsec */
        return 0;
    }
    
    case XV6_CLOCK_MONOTONIC:
    case XV6_CLOCK_MONOTONIC_RAW:
    case XV6_CLOCK_MONOTONIC_COARSE:
    case XV6_CLOCK_PROCESS_CPUTIME_ID:
    case XV6_CLOCK_THREAD_CPUTIME_ID: {
        /* Use uptime syscall which returns jiffies (ms since boot at HZ=1000) */
        uint64_t jiffies = _xv6_uptime();
        tp->tv_sec = jiffies / 1000;
        tp->tv_nsec = (jiffies % 1000) * 1000000;  /* ms to nsec */
        return 0;
    }
    
    default:
        errno = EINVAL;
        return -1;
    }
}

/* clock_getres - get clock resolution */
int clock_getres(clockid_t clk_id, struct timespec *res) {
    if (res == NULL) {
        return 0;  /* NULL is allowed per POSIX */
    }
    
    switch (clk_id) {
    case XV6_CLOCK_REALTIME_COARSE:
    case XV6_CLOCK_REALTIME:
        /* gettimeofday has microsecond resolution */
        res->tv_sec = 0;
        res->tv_nsec = 1000;  /* 1 microsecond */
        return 0;
    
    case XV6_CLOCK_MONOTONIC:
    case XV6_CLOCK_MONOTONIC_RAW:
    case XV6_CLOCK_MONOTONIC_COARSE:
    case XV6_CLOCK_PROCESS_CPUTIME_ID:
    case XV6_CLOCK_THREAD_CPUTIME_ID:
        /* uptime has millisecond resolution (HZ=1000) */
        res->tv_sec = 0;
        res->tv_nsec = 1000000;  /* 1 millisecond */
        return 0;
    
    default:
        errno = EINVAL;
        return -1;
    }
}

/*
 * Misc stubs
 */

int _chown(const char *path, uid_t owner, gid_t group) {
    /* xv6 doesn't support ownership */
    (void)path;
    (void)owner;
    (void)group;
    errno = ENOSYS;
    return -1;
}

int _getentropy(void *buffer, size_t length) {
    /*
     * Use xv6's getrandom syscall for entropy.
     */
    if (buffer == NULL || length > 256) {
        errno = EIO;
        return -1;
    }
    
    ssize_t ret = getrandom(buffer, length, 0);
    if (ret < 0) {
        /* getrandom returns -errno on error */
        if (ret < -1) {
            errno = -ret;
        }
        return -1;
    }
    
    return 0;
}

/*
 * Additional syscalls for CPython compatibility
 */

int _fcntl(int fd, int cmd, ...) {
    /* For simplicity, assume third arg is int */
    /* A proper implementation would use va_args */
    int arg = 0;
    return _xv6_fcntl(fd, cmd, arg);
}

int _access(const char *path, int mode) {
    return _xv6_access(path, mode);
}

pid_t _waitpid(pid_t pid, int *status, int options) {
    return _xv6_waitpid(pid, status, options);
}

int _nanosleep(const struct timespec *req, struct timespec *rem) {
    return _xv6_nanosleep(req, rem);
}

int _ftruncate(int fd, off_t length) {
    return _xv6_ftruncate(fd, (int64_t)length);
}

int _ftruncate64(int fd, _off64_t length) {
    if (length > (_off64_t)INT64_MAX || length < (_off64_t)INT64_MIN) {
        errno = EOVERFLOW;
        return -1;
    }
    return _xv6_ftruncate(fd, (int64_t)length);
}

int _truncate(const char *path, off_t length) {
    int fd = _open(path, NEWLIB_O_WRONLY, 0);
    if (fd < 0) {
        return -1;
    }
    int ret = _ftruncate(fd, length);
    int close_ret = _close(fd);
    if (ret < 0) {
        return ret;
    }
    return close_ret;
}

int _truncate64(const char *path, _off64_t length) {
    int fd = _open(path, NEWLIB_O_WRONLY, 0);
    if (fd < 0) {
        return -1;
    }
    int ret = _ftruncate64(fd, length);
    int close_ret = _close(fd);
    if (ret < 0) {
        return ret;
    }
    return close_ret;
}

int _rename(const char *oldpath, const char *newpath) {
    return _xv6_rename(oldpath, newpath);
}

pid_t _getppid(void) {
    return _xv6_getppid();
}

/* Stub functions that return constant values */
uid_t _getuid(void) { return 0; }
uid_t _geteuid(void) { return 0; }
gid_t _getgid(void) { return 0; }
gid_t _getegid(void) { return 0; }
mode_t _umask(mode_t mask) { (void)mask; return 022; }
int _chmod(const char *path, mode_t mode) { (void)path; (void)mode; return 0; }
int _fchmod(int fd, mode_t mode) { (void)fd; (void)mode; return 0; }
int _setuid(uid_t uid) { (void)uid; return 0; }
int _setgid(gid_t gid) { (void)gid; return 0; }
int fsync(int fd) { (void)fd; return 0; }
int fdatasync(int fd) { (void)fd; return 0; }

/*
 * Note: getrandom is now provided by usys.S as a syscall stub.
 * Python and other code can call it directly.
 */

/*
 * mmap/munmap - Memory mapping functions
 * 
 * Note: xv6 provides mmap/munmap directly via usys.S syscall stubs.
 * We provide _mmap/_munmap for newlib's internal convention.
 * xv6 uses Linux-compatible flag values.
 */
void *_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return _xv6_mmap(addr, (uint64_t)length, prot, flags, fd, (uint64_t)offset);
}

int _munmap(void *addr, size_t length) {
    return _xv6_munmap(addr, (uint64_t)length);
}

int _mprotect(void *addr, size_t length, int prot) {
    return _xv6_mprotect(addr, (uint64_t)length, prot);
}

/*
 * Signal handling functions
 * 
 * Note: sigaction and sigprocmask are defined in usys.S as syscall stubs.
 * We provide wrapper functions with underscore prefix for newlib compatibility.
 * The sigset manipulation functions are pure user-space operations.
 */
int _sigaction(int signum, const void *act, void *oldact) {
    /* Pass through - caller is responsible for correct struct layout */
    return _xv6_sigaction(signum, (struct xv6_sigaction *)act, (struct xv6_sigaction *)oldact);
}

int _sigprocmask(int how, const void *set, void *oldset) {
    return _xv6_sigprocmask(how, (const uint64_t *)set, (uint64_t *)oldset);
}

/* signal() is a simplified wrapper around sigaction */
void (*_signal(int signum, void (*handler)(int)))(int) {
    struct xv6_sigaction act, oldact;
    act.sa_handler = handler;
    act.sa_mask = 0;
    act.sa_flags = 0;
    
    if (_xv6_sigaction(signum, &act, &oldact) < 0) {
        return (void (*)(int))-1;  /* SIG_ERR */
    }
    return oldact.sa_handler;
}
/*
 * Directory functions - opendir/readdir/closedir/rewinddir
 * 
 * These implement POSIX directory operations using the getdents syscall.
 * The DIR and struct dirent types are defined in sys/dirent.h
 */

/* Include our dirent header for DIR and struct dirent */
#include <sys/dirent.h>

/* xv6 getdents syscall */
extern int _xv6_getdents(int fd, void *dirp, int count) __asm__("getdents");

/* Linux-compatible dirent64 structure (matches kernel's output format) */
struct linux_dirent64 {
    uint64_t d_ino;      /* Inode number */
    int64_t  d_off;      /* Offset to next structure */
    uint16_t d_reclen;   /* Size of this dirent */
    uint8_t  d_type;     /* File type */
    char     d_name[];   /* Filename (null-terminated) */
};

DIR *opendir(const char *name) {
    int fd = _xv6_open(name, 0);  /* O_RDONLY = 0 in xv6 */
    if (fd < 0) {
        errno = ENOENT;
        return NULL;
    }
    
    /* Allocate DIR structure */
    DIR *dir = (DIR *)malloc(sizeof(DIR));
    if (dir == NULL) {
        _xv6_close(fd);
        errno = ENOMEM;
        return NULL;
    }
    
    dir->dd_fd = fd;
    dir->dd_loc = 0;
    dir->dd_size = 0;
    
    return dir;
}

struct dirent *readdir(DIR *dir) {
    if (dir == NULL) {
        errno = EBADF;
        return NULL;
    }
    
    /* Need to read more entries? */
    if (dir->dd_loc >= dir->dd_size) {
        int nread = _xv6_getdents(dir->dd_fd, dir->dd_buf, _DIR_BUF_SIZE);
        if (nread <= 0) {
            if (nread < 0) {
                errno = EIO;
            }
            return NULL;  /* End of directory or error */
        }
        dir->dd_loc = 0;
        dir->dd_size = nread;
    }
    
    /* Get current entry from buffer */
    struct linux_dirent64 *de = (struct linux_dirent64 *)(dir->dd_buf + dir->dd_loc);
    
    /* Copy to our dirent structure */
    dir->dd_ent.d_ino = de->d_ino;
    dir->dd_ent.d_off = de->d_off;
    dir->dd_ent.d_reclen = de->d_reclen;
    dir->dd_ent.d_type = de->d_type;
    
    /* Copy name (ensure null termination) */
    size_t name_len = de->d_reclen - offsetof(struct linux_dirent64, d_name) - 1;
    if (name_len > 255) name_len = 255;
    memcpy(dir->dd_ent.d_name, de->d_name, name_len);
    dir->dd_ent.d_name[name_len] = '\0';
    
    /* Advance to next entry */
    dir->dd_loc += de->d_reclen;
    
    return &dir->dd_ent;
}

int closedir(DIR *dir) {
    if (dir == NULL) {
        errno = EBADF;
        return -1;
    }
    
    int ret = _xv6_close(dir->dd_fd);
    free(dir);
    return ret;
}

void rewinddir(DIR *dir) {
    if (dir == NULL) {
        return;
    }
    _xv6_lseek(dir->dd_fd, 0, 0);  /* SEEK_SET = 0 */
    dir->dd_loc = 0;
    dir->dd_size = 0;
}

/* dirfd - get file descriptor from DIR */
int dirfd(DIR *dir) {
    if (dir == NULL) {
        errno = EINVAL;
        return -1;
    }
    return dir->dd_fd;
}

/* ============================================================================
 * Environment Variables
 * ============================================================================
 * 
 * Newlib expects a global 'environ' variable pointing to an array of
 * "NAME=VALUE" strings, terminated by NULL. We provide getenv, setenv,
 * unsetenv, and putenv.
 */

#define MAX_ENV_VARS 64
#define MAX_ENV_ENTRY 512  /* Max length of a single "NAME=VALUE" string */

/* Storage for environment strings */
static char env_storage[MAX_ENV_VARS][MAX_ENV_ENTRY];
/* Pointer array for environ */
static char *env_ptrs[MAX_ENV_VARS + 1];

/* 
 * The environ global that newlib/POSIX expects.
 * This is declared extern in <unistd.h> or <stdlib.h>.
 */
char **environ = env_ptrs;

/* Track initialization */
static int env_initialized = 0;

/* Pointer to envp passed by the kernel via the stack, extracted by crt0.
 * Weak symbol so configure tests (which don't link crt0) get NULL default. */
char **__xv6_envp __attribute__((weak)) = NULL;

/* Initialize environment from kernel-provided envp, then set defaults */
static void env_init(void) {
    if (env_initialized) return;
    env_initialized = 1;
    
    /* Clear all entries */
    for (int i = 0; i <= MAX_ENV_VARS; i++) {
        env_ptrs[i] = NULL;
    }
    
    /* Import environment variables passed by the kernel (via execve) */
    if (__xv6_envp) {
        for (int i = 0; __xv6_envp[i] != NULL && i < MAX_ENV_VARS; i++) {
            int len = strlen(__xv6_envp[i]);
            if (len >= MAX_ENV_ENTRY)
                len = MAX_ENV_ENTRY - 1;
            memcpy(env_storage[i], __xv6_envp[i], len);
            env_storage[i][len] = '\0';
            env_ptrs[i] = env_storage[i];
        }
    }
    
    /* Set defaults only if not already inherited */
    if (!getenv("PATH"))  setenv("PATH", "/:/bin", 1);
    if (!getenv("HOME"))  setenv("HOME", "/", 1);
    if (!getenv("USER"))  setenv("USER", "root", 1);
    if (!getenv("SHELL")) setenv("SHELL", "/sh", 1);
    if (!getenv("TERM"))  setenv("TERM", "xterm", 1);
    if (!getenv("PWD"))   setenv("PWD", "/", 1);
    
    /* Python configuration */
    if (!getenv("PYTHONDONTWRITEBYTECODE")) setenv("PYTHONDONTWRITEBYTECODE", "1", 1);
}

/* Find an environment variable by name, returns index or -1 */
static int env_find(const char *name) {
    if (!name) return -1;
    
    size_t name_len = 0;
    while (name[name_len] && name[name_len] != '=') {
        name_len++;
    }
    
    for (int i = 0; env_ptrs[i] != NULL; i++) {
        /* Check if the entry starts with "name=" */
        const char *entry = env_ptrs[i];
        size_t j;
        for (j = 0; j < name_len; j++) {
            if (entry[j] != name[j]) break;
        }
        if (j == name_len && entry[j] == '=') {
            return i;
        }
    }
    return -1;
}

/* Count current environment entries */
static int env_count(void) {
    int count = 0;
    while (env_ptrs[count] != NULL) {
        count++;
    }
    return count;
}

/* 
 * getenv - Get value of environment variable
 * Returns pointer to value part (after '='), or NULL if not found.
 */
char *getenv(const char *name) {
    if (!env_initialized) env_init();
    
    if (!name || !*name) return NULL;
    
    int idx = env_find(name);
    if (idx < 0) return NULL;
    
    /* Find the '=' and return pointer to what follows */
    char *entry = env_ptrs[idx];
    while (*entry && *entry != '=') {
        entry++;
    }
    if (*entry == '=') {
        return entry + 1;
    }
    return NULL;
}

/*
 * setenv - Set environment variable
 * If overwrite is 0, don't change existing variable.
 * Returns 0 on success, -1 on error.
 */
int setenv(const char *name, const char *value, int overwrite) {
    if (!env_initialized) env_init();
    
    if (!name || !*name || strchr(name, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }
    
    if (!value) value = "";
    
    size_t name_len = strlen(name);
    size_t value_len = strlen(value);
    
    if (name_len + value_len + 2 > MAX_ENV_ENTRY) {
        errno = ENOMEM;
        return -1;
    }
    
    int idx = env_find(name);
    
    if (idx >= 0) {
        /* Variable exists */
        if (!overwrite) {
            return 0;  /* Don't overwrite, but not an error */
        }
        /* Overwrite in place - find which storage slot this is */
        for (int i = 0; i < MAX_ENV_VARS; i++) {
            if (env_ptrs[idx] == env_storage[i]) {
                /* Format new value into this slot */
                char *p = env_storage[i];
                memcpy(p, name, name_len);
                p[name_len] = '=';
                memcpy(p + name_len + 1, value, value_len);
                p[name_len + 1 + value_len] = '\0';
                return 0;
            }
        }
        /* Not in our storage? Shouldn't happen, but handle gracefully */
        return -1;
    }
    
    /* Variable doesn't exist - find a free slot */
    int count = env_count();
    if (count >= MAX_ENV_VARS) {
        errno = ENOMEM;
        return -1;
    }
    
    /* Find an unused storage slot */
    for (int i = 0; i < MAX_ENV_VARS; i++) {
        int used = 0;
        for (int j = 0; env_ptrs[j] != NULL; j++) {
            if (env_ptrs[j] == env_storage[i]) {
                used = 1;
                break;
            }
        }
        if (!used) {
            /* Format new entry */
            char *p = env_storage[i];
            memcpy(p, name, name_len);
            p[name_len] = '=';
            memcpy(p + name_len + 1, value, value_len);
            p[name_len + 1 + value_len] = '\0';
            
            /* Add to environ array */
            env_ptrs[count] = env_storage[i];
            env_ptrs[count + 1] = NULL;
            return 0;
        }
    }
    
    errno = ENOMEM;
    return -1;
}

/*
 * unsetenv - Remove environment variable
 * Returns 0 on success, -1 on error.
 */
int unsetenv(const char *name) {
    if (!env_initialized) env_init();
    
    if (!name || !*name || strchr(name, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }
    
    int idx = env_find(name);
    if (idx < 0) {
        return 0;  /* Not found is not an error */
    }
    
    /* Shift remaining entries down */
    int count = env_count();
    for (int i = idx; i < count - 1; i++) {
        env_ptrs[i] = env_ptrs[i + 1];
    }
    env_ptrs[count - 1] = NULL;
    
    return 0;
}

/*
 * putenv - Put "NAME=VALUE" string into environment
 * Note: The string is NOT copied - it must persist!
 * For safety, we copy it into our storage.
 * Returns 0 on success, non-zero on error.
 */
int putenv(char *string) {
    if (!env_initialized) env_init();
    
    if (!string) {
        errno = EINVAL;
        return -1;
    }
    
    char *eq = strchr(string, '=');
    if (!eq) {
        /* No '=' means unset the variable */
        return unsetenv(string);
    }
    
    /* Extract name and value */
    size_t name_len = eq - string;
    const char *value = eq + 1;
    
    /* Create a temporary null-terminated name */
    char name[MAX_ENV_ENTRY];
    if (name_len >= MAX_ENV_ENTRY) {
        errno = ENOMEM;
        return -1;
    }
    memcpy(name, string, name_len);
    name[name_len] = '\0';
    
    return setenv(name, value, 1);
}

/*
 * clearenv - Clear entire environment
 * Returns 0 on success.
 */
int clearenv(void) {
    if (!env_initialized) env_init();
    
    for (int i = 0; i <= MAX_ENV_VARS; i++) {
        env_ptrs[i] = NULL;
    }
    return 0;
}

/* ============================================================================
 * I/O Multiplexing: select() and poll()
 * ============================================================================
 *
 * select() converts fd_sets into a pollfd array and delegates to the
 * kernel's poll syscall (_xv6_poll), which performs real readiness
 * checking for pipes, character devices (console/TTY), regular files, etc.
 *
 * poll() is a thin wrapper around the kernel syscall.
 */

/* Use newlib's fd_set definition from sys/select.h */
#include <sys/select.h>

/* poll structures - use our own since newlib may not have poll.h */
#ifndef POLLIN
struct pollfd {
    int fd;
    short events;
    short revents;
};

#define POLLIN      0x0001
#define POLLPRI     0x0002
#define POLLOUT     0x0004
#define POLLERR     0x0008
#define POLLHUP     0x0010
#define POLLNVAL    0x0020
#define POLLRDNORM  0x0040
#define POLLWRNORM  0x0100
#endif

/*
 * select - synchronous I/O multiplexing
 *
 * Converts the fd_set bitmasks into a pollfd array, invokes the kernel
 * poll syscall, then converts the results back into fd_sets.
 */
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout) {
    if (nfds < 0 || nfds > FD_SETSIZE) {
        errno = EINVAL;
        return -1;
    }

    /* Convert timeout to milliseconds for poll() */
    int timeout_ms;
    if (timeout == NULL) {
        timeout_ms = -1;   /* block indefinitely */
    } else if (timeout->tv_sec == 0 && timeout->tv_usec == 0) {
        timeout_ms = 0;    /* non-blocking poll */
    } else {
        timeout_ms = (int)(timeout->tv_sec * 1000 +
                           timeout->tv_usec / 1000);
        if (timeout_ms <= 0)
            timeout_ms = 1; /* at least 1 ms for sub-ms timeouts */
    }

    /*
     * Build the pollfd array.  Each fd that appears in any of the
     * fd_sets gets one entry with the appropriate event flags.
     */
    struct pollfd pfds[FD_SETSIZE];
    int npfds = 0;

    for (int fd = 0; fd < nfds; fd++) {
        short events = 0;
        if (readfds  && FD_ISSET(fd, readfds))   events |= POLLIN;
        if (writefds && FD_ISSET(fd, writefds))  events |= POLLOUT;
        if (exceptfds && FD_ISSET(fd, exceptfds)) events |= POLLPRI;
        if (events) {
            pfds[npfds].fd      = fd;
            pfds[npfds].events  = events;
            pfds[npfds].revents = 0;
            npfds++;
        }
    }

    if (npfds == 0) {
        /* No fds requested — just sleep for the timeout duration */
        if (timeout_ms > 0) {
            struct timespec ts;
            ts.tv_sec  = timeout_ms / 1000;
            ts.tv_nsec = (timeout_ms % 1000) * 1000000L;
            _xv6_nanosleep(&ts, NULL);
        }
        return 0;
    }

    /* Ask the kernel */
    int ret = _xv6_poll((void *)pfds, npfds, timeout_ms);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    /* Clear fd_sets and rebuild from revents */
    if (readfds)   FD_ZERO(readfds);
    if (writefds)  FD_ZERO(writefds);
    if (exceptfds) FD_ZERO(exceptfds);

    int count = 0;
    for (int i = 0; i < npfds; i++) {
        short rev = pfds[i].revents;
        int   fd  = pfds[i].fd;
        int   got = 0;

        if (readfds && (rev & (POLLIN | POLLHUP | POLLERR))) {
            FD_SET(fd, readfds);
            got = 1;
        }
        if (writefds && (rev & (POLLOUT | POLLERR))) {
            FD_SET(fd, writefds);
            got = 1;
        }
        if (exceptfds && (rev & (POLLPRI | POLLNVAL))) {
            FD_SET(fd, exceptfds);
            got = 1;
        }
        count += got;
    }

    return count;
}

/*
 * pselect - synchronous I/O multiplexing (POSIX)
 *
 * Thin wrapper around select().
 * Ignores the signal mask (sigmask) since xv6 signal support is minimal.
 */
int pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask) {
    (void)sigmask;
    struct timeval tv, *tvp = NULL;
    if (timeout) {
        tv.tv_sec = timeout->tv_sec;
        tv.tv_usec = timeout->tv_nsec / 1000;
        tvp = &tv;
    }
    return select(nfds, readfds, writefds, exceptfds, tvp);
}

/*
 * poll - wait for events on file descriptors
 *
 * Delegates directly to the kernel poll syscall.
 */
int poll(struct pollfd *fds, unsigned long nfds, int timeout) {
    if (nfds > (unsigned long)INT_MAX) {
        errno = EINVAL;
        return -1;
    }

    int ret = _xv6_poll((void *)fds, (int)nfds, timeout);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}

/* ============================================================================
 * ioctl - I/O control
 * ============================================================================
 * Note: The basic ioctl syscall is provided by usys.S.
 * We don't need a wrapper here since usys.S provides it directly.
 * For newlib programs that need errno handling, use this weak alias approach.
 */

/* ============================================================================
 * Resource Limits: getrlimit/setrlimit
 * ============================================================================
 * 
 * Stub implementation - xv6 doesn't have resource limits.
 * Return reasonable defaults that won't break applications.
 */

typedef uint64_t rlim_t;

struct rlimit {
    rlim_t rlim_cur;  /* Soft limit */
    rlim_t rlim_max;  /* Hard limit */
};

#define RLIM_INFINITY (~0ULL)

/* Resource types */
#define RLIMIT_CPU        0
#define RLIMIT_FSIZE      1
#define RLIMIT_DATA       2
#define RLIMIT_STACK      3
#define RLIMIT_CORE       4
#define RLIMIT_RSS        5
#define RLIMIT_NPROC      6
#define RLIMIT_NOFILE     7
#define RLIMIT_MEMLOCK    8
#define RLIMIT_AS         9
#define RLIMIT_LOCKS      10
#define RLIMIT_SIGPENDING 11
#define RLIMIT_MSGQUEUE   12
#define RLIMIT_NICE       13
#define RLIMIT_RTPRIO     14
#define RLIMIT_RTTIME     15
#define RLIMIT_NLIMITS    16

int getrlimit(int resource, struct rlimit *rlim) {
    if (rlim == NULL) {
        errno = EFAULT;
        return -1;
    }
    
    if (resource < 0 || resource >= RLIMIT_NLIMITS) {
        errno = EINVAL;
        return -1;
    }
    
    /* Return sensible defaults */
    switch (resource) {
    case RLIMIT_NOFILE:
        rlim->rlim_cur = 256;       /* Reasonable file descriptor limit */
        rlim->rlim_max = 1024;
        break;
    case RLIMIT_STACK:
        rlim->rlim_cur = 8 * 1024 * 1024;  /* 8MB stack */
        rlim->rlim_max = RLIM_INFINITY;
        break;
    case RLIMIT_DATA:
    case RLIMIT_AS:
        rlim->rlim_cur = RLIM_INFINITY;    /* Unlimited */
        rlim->rlim_max = RLIM_INFINITY;
        break;
    default:
        rlim->rlim_cur = RLIM_INFINITY;
        rlim->rlim_max = RLIM_INFINITY;
        break;
    }
    
    return 0;
}

int setrlimit(int resource, const struct rlimit *rlim) {
    (void)resource;
    (void)rlim;
    /* Silently succeed - we don't actually enforce limits */
    return 0;
}

/* prlimit - combined get/set resource limits (Linux extension) */
int prlimit(pid_t pid, int resource, const struct rlimit *new_limit,
            struct rlimit *old_limit) {
    (void)pid;  /* Ignore pid - always operate on current process */
    
    if (old_limit) {
        if (getrlimit(resource, old_limit) < 0) {
            return -1;
        }
    }
    
    if (new_limit) {
        if (setrlimit(resource, new_limit) < 0) {
            return -1;
        }
    }
    
    return 0;
}

/*
 * memrchr - like memchr but searches from the end
 * Returns pointer to last occurrence of c in s, or NULL if not found.
 */
void *memrchr(const void *s, int c, size_t n) {
    const unsigned char *p = (const unsigned char *)s + n;
    const unsigned char uc = (unsigned char)c;
    
    while (n-- > 0) {
        --p;
        if (*p == uc) {
            return (void *)p;
        }
    }
    return 0;
}

/*
 * uname - get system identification
 * Note: uname is implemented as a kernel syscall (sys_uname).
 * The syscall stub is generated by usys.pl.
 */
#include "sys/utsname.h"

/*
 * utime - set file access and modification times
 * Old-style interface that takes time_t[2]
 */
int utime(const char *path, const long *times) {
    (void)path;
    (void)times;
    /* Silently succeed - xv6 doesn't track access/modification times */
    return 0;
}

/* CPython POSIX stubs - functions not supported on xv6 */
int setgid(gid_t gid) { errno = ENOSYS; return -1; }
int sched_yield(void) { return 0; }  /* success, no-op */
int execv(const char *path, char *const argv[]) { errno = ENOSYS; return -1; }
int fchdir(int fd) { errno = ENOSYS; return -1; }
int rmdir(const char *path) { return unlink(path); }  /* xv6 uses unlink for dirs */
uid_t getuid(void) { return 0; }
gid_t getgid(void) { return 0; }
uid_t geteuid(void) { return 0; }
gid_t getegid(void) { return 0; }
int setuid(uid_t uid) { errno = ENOSYS; return -1; }
int setgroups(int ngroups, const gid_t *grouplist) { errno = ENOSYS; return -1; }

/* ===================================================================== */
/* Network / socket wrappers                                             */
/* ===================================================================== */

#include <sys/socket.h>
#include <netinet/in.h>

/*
 * Socket wrappers are called directly (not through newlib's _xxx_r layer),
 * so they must set the real newlib errno — not the global 'int errno' that
 * was #undef'd above for the reentrant syscall stubs.
 */
extern int *__errno(void);
#undef errno
#define errno (*__errno())

int socket(int domain, int type, int protocol) {
    int ret = _xv6_socket(domain, type, protocol);
    if (ret < 0) { errno = -ret; return -1; }
    return ret;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int ret = _xv6_bind(sockfd, addr, (int)addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

int listen(int sockfd, int backlog) {
    int ret = _xv6_listen(sockfd, backlog);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int ret = _xv6_accept(sockfd, addr, addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return ret;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int ret = _xv6_sconnect(sockfd, addr, (int)addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen) {
    int ret = _xv6_sendto(sockfd, buf, (int)len, flags, dest_addr, (int)addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return ret;
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {
    int ret = _xv6_recvfrom(sockfd, buf, (int)len, flags, src_addr, addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return ret;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return sendto(sockfd, buf, len, flags, NULL, 0);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return recvfrom(sockfd, buf, len, flags, NULL, NULL);
}

int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen) {
    int ret = _xv6_setsockopt(sockfd, level, optname, optval, (int)optlen);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

int getsockopt(int sockfd, int level, int optname,
               void *optval, socklen_t *optlen) {
    int ret = _xv6_getsockopt(sockfd, level, optname, optval, optlen);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

int shutdown(int sockfd, int how) {
    int ret = _xv6_shutdown(sockfd, how);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int ret = _xv6_getpeername(sockfd, addr, addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int ret = _xv6_getsockname(sockfd, addr, addrlen);
    if (ret < 0) { errno = -ret; return -1; }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  inet address conversion functions (needed by CPython _socket)     */
/* ------------------------------------------------------------------ */

#include <stdio.h>   /* sscanf, snprintf */
#include <netdb.h>   /* struct addrinfo, getaddrinfo decl */

/* DNS error code variable (declared extern in netdb.h) */
int h_errno = 0;

/* Byte order: RISC-V is little-endian, network is big-endian.
 * Provide function versions alongside the macros in netinet/in.h so
 * that code taking the address of htons/ntohl etc. can link.
 */
#undef htons
#undef ntohs
#undef htonl
#undef ntohl
uint16_t htons(uint16_t x) { return __builtin_bswap16(x); }
uint16_t ntohs(uint16_t x) { return __builtin_bswap16(x); }
uint32_t htonl(uint32_t x) { return __builtin_bswap32(x); }
uint32_t ntohl(uint32_t x) { return __builtin_bswap32(x); }

/* Forward declare so inet_addr can call inet_aton */
int inet_aton(const char *cp, struct in_addr *inp);

/**
 * Parse a dotted-quad IPv4 address string into a network-byte-order uint32.
 * Returns INADDR_NONE on error.
 */
in_addr_t inet_addr(const char *cp)
{
    struct in_addr val;
    if (inet_aton(cp, &val))
        return val.s_addr;
    return INADDR_NONE;
}

/**
 * Parse a dotted-quad IPv4 address string.
 * Returns 1 on success, 0 on failure.
 */
int inet_aton(const char *cp, struct in_addr *inp)
{
    unsigned int a, b, c, d;
    int n;
    /* accept "1.2.3.4" only — no octal/hex shortcuts */
    n = sscanf(cp, "%u.%u.%u.%u", &a, &b, &c, &d);
    if (n != 4)
        return 0;
    if (a > 255 || b > 255 || c > 255 || d > 255)
        return 0;
    inp->s_addr = htonl((a << 24) | (b << 16) | (c << 8) | d);
    return 1;
}

/**
 * Convert a network-byte-order IPv4 address to dotted-quad string.
 * Uses a static buffer (not thread safe — matches POSIX spec).
 */
char *inet_ntoa(struct in_addr in)
{
    static char buf[16];  /* "255.255.255.255\0" */
    uint32_t addr = ntohl(in.s_addr);
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
             (addr >> 24) & 0xff,
             (addr >> 16) & 0xff,
             (addr >>  8) & 0xff,
              addr        & 0xff);
    return buf;
}

/**
 * Convert text address to binary form.
 * Only AF_INET is fully supported; AF_INET6 is stubbed.
 */
int inet_pton(int af, const char *src, void *dst)
{
    if (af == AF_INET) {
        struct in_addr tmp;
        if (inet_aton(src, &tmp)) {
            memcpy(dst, &tmp, sizeof(tmp));
            return 1;
        }
        return 0;
    }
    errno = EAFNOSUPPORT;
    return -1;
}

/**
 * Convert binary address to text form.
 * Only AF_INET is supported.
 */
const char *inet_ntop(int af, const void *src, char *dst, unsigned int size)
{
    if (af == AF_INET) {
        struct in_addr tmp;
        memcpy(&tmp, src, sizeof(tmp));
        uint32_t addr = ntohl(tmp.s_addr);
        int n = snprintf(dst, size, "%u.%u.%u.%u",
                         (addr >> 24) & 0xff,
                         (addr >> 16) & 0xff,
                         (addr >>  8) & 0xff,
                          addr        & 0xff);
        if (n < 0 || (unsigned int)n >= size) {
            errno = ENOSPC;
            return NULL;
        }
        return dst;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  DNS / name resolution stubs                                       */
/*  xv6 has no DNS resolver; only numeric IPv4 addresses work.        */
/* ------------------------------------------------------------------ */

/**
 * getaddrinfo — resolve host/service to socket address.
 *
 * Only numeric IPv4 addresses and numeric ports are supported.
 * DNS names will return EAI_NONAME.
 */
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints, struct addrinfo **res)
{
    if (!res)
        return EAI_FAIL;
    *res = NULL;

    int family = hints ? hints->ai_family : AF_INET;
    int socktype = hints ? hints->ai_socktype : SOCK_STREAM;
    int protocol = hints ? hints->ai_protocol : 0;

    struct sockaddr_in *sa = (struct sockaddr_in *)malloc(sizeof(*sa));
    struct addrinfo *ai = (struct addrinfo *)malloc(sizeof(*ai));
    if (!sa || !ai) {
        free(sa);
        free(ai);
        return EAI_MEMORY;
    }

    memset(sa, 0, sizeof(*sa));
    sa->sin_family = AF_INET;

    /* Parse port */
    if (service) {
        int port = 0;
        for (const char *p = service; *p; p++) {
            if (*p < '0' || *p > '9') {
                free(sa); free(ai);
                return EAI_SERVICE;
            }
            port = port * 10 + (*p - '0');
        }
        sa->sin_port = htons((uint16_t)port);
    }

    /* Parse address */
    if (node) {
        if (!inet_aton(node, &sa->sin_addr)) {
            /* Not a numeric address — no DNS on xv6 */
            free(sa); free(ai);
            return EAI_NONAME;
        }
    } else if (hints && (hints->ai_flags & AI_PASSIVE)) {
        sa->sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }

    memset(ai, 0, sizeof(*ai));
    ai->ai_flags = hints ? hints->ai_flags : 0;
    ai->ai_family = (family == AF_UNSPEC) ? AF_INET : family;
    ai->ai_socktype = socktype;
    ai->ai_protocol = protocol;
    ai->ai_addrlen = sizeof(*sa);
    ai->ai_canonname = NULL;
    ai->ai_addr = (struct sockaddr *)sa;
    ai->ai_next = NULL;

    *res = ai;
    return 0;
}

void freeaddrinfo(struct addrinfo *res)
{
    while (res) {
        struct addrinfo *next = res->ai_next;
        free(res->ai_addr);
        free(res->ai_canonname);
        free(res);
        res = next;
    }
}

const char *gai_strerror(int errcode)
{
    switch (errcode) {
    case 0:             return "Success";
    case EAI_NONAME:    return "Name or service not known";
    case EAI_AGAIN:     return "Temporary failure in name resolution";
    case EAI_FAIL:      return "Non-recoverable failure in name resolution";
    case EAI_FAMILY:    return "Address family not supported";
    case EAI_SOCKTYPE:  return "Socket type not supported";
    case EAI_SERVICE:   return "Service not supported";
    case EAI_MEMORY:    return "Memory allocation failure";
    case EAI_SYSTEM:    return "System error";
    default:            return "Unknown error";
    }
}

int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags)
{
    (void)flags;
    if (sa->sa_family != AF_INET)
        return EAI_FAMILY;

    const struct sockaddr_in *sin = (const struct sockaddr_in *)sa;
    (void)salen;

    if (host && hostlen > 0) {
        inet_ntop(AF_INET, &sin->sin_addr, host, hostlen);
    }
    if (serv && servlen > 0) {
        snprintf(serv, servlen, "%d", ntohs(sin->sin_port));
    }
    return 0;
}

/* gethostbyname - minimal stub, only handles numeric addresses */
struct hostent *gethostbyname(const char *name)
{
    static struct hostent he;
    static struct in_addr addr;
    static char *addr_list[2];
    static char hostname[64];

    if (!inet_aton(name, &addr)) {
        h_errno = HOST_NOT_FOUND;
        return NULL;
    }

    strncpy(hostname, name, sizeof(hostname) - 1);
    hostname[sizeof(hostname) - 1] = '\0';
    addr_list[0] = (char *)&addr;
    addr_list[1] = NULL;

    he.h_name = hostname;
    he.h_aliases = NULL;
    he.h_addrtype = AF_INET;
    he.h_length = sizeof(struct in_addr);
    he.h_addr_list = addr_list;

    return &he;
}

struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
    (void)addr; (void)len; (void)type;
    h_errno = HOST_NOT_FOUND;
    return NULL;
}

struct protoent *getprotobyname(const char *name)
{
    static struct protoent pe;
    static char *no_aliases[] = { NULL };

    if (strcmp(name, "tcp") == 0) {
        pe.p_name = "tcp"; pe.p_aliases = no_aliases; pe.p_proto = 6;
        return &pe;
    }
    if (strcmp(name, "udp") == 0) {
        pe.p_name = "udp"; pe.p_aliases = no_aliases; pe.p_proto = 17;
        return &pe;
    }
    return NULL;
}

struct servent *getservbyname(const char *name, const char *proto)
{
    (void)name; (void)proto;
    return NULL;
}

struct servent *getservbyport(int port, const char *proto)
{
    (void)port; (void)proto;
    return NULL;
}
