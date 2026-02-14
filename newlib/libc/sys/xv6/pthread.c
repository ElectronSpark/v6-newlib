/*
 * pthread.c - POSIX threads implementation for xv6
 * 
 * Uses xv6's clone() syscall with CLONE_VM to create true threads.
 * 
 * This file avoids including newlib headers to prevent type conflicts
 * with kernel headers. It uses matching type definitions.
 */

/* Basic type definitions matching newlib and the kernel */
typedef unsigned long size_t;
typedef long ssize_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef int int32_t;
typedef int pid_t;

/* Pthread types matching newlib's definitions */
typedef uint32_t pthread_t;
typedef uint32_t pthread_mutex_t;
typedef uint32_t pthread_cond_t;
typedef uint32_t pthread_key_t;

/* Pthread attribute types - newlib defines these as structs */
typedef struct {
    int is_initialized;
    void *stackaddr;
    int stacksize;
    int contentionscope;
    int inheritsched;
    int schedpolicy;
    int detachstate;
} pthread_attr_t;

typedef struct {
    int is_initialized;
    int process_shared;
} pthread_mutexattr_t;

typedef struct {
    int is_initialized;
    int process_shared;
} pthread_condattr_t;

typedef struct {
    int is_initialized;
} pthread_once_t;

/* Timespec for nanosleep */
struct timespec {
    long tv_sec;
    long tv_nsec;
};

/* Constants */
#define NULL ((void *)0)
#define EINVAL 22
#define EAGAIN 11
#define ENOMEM 12
#define ESRCH  3
#define EBUSY  16
#define ENOTSUP 95

#define PTHREAD_CREATE_JOINABLE 1
#define PTHREAD_CREATE_DETACHED 0
#define PTHREAD_CANCEL_ENABLE   0
#define PTHREAD_CANCEL_DISABLE  1
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_SCOPE_SYSTEM    0
#define PTHREAD_SCOPE_PROCESS   1
#define PTHREAD_MUTEX_NORMAL    0
#define PTHREAD_MUTEX_INITIALIZER 0xFFFFFFFF
#define PTHREAD_COND_INITIALIZER  0xFFFFFFFF

/* External functions - declared to avoid header conflicts */
extern void *memset(void *s, int c, size_t n);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern pid_t getpid(void);
extern void _exit(int status) __attribute__((noreturn));
extern int waitpid(pid_t pid, int *status, int options);
extern int nanosleep(const struct timespec *req, struct timespec *rem);

/* Clone flags - must match kernel/inc/clone_flags.h */
#define CLONE_VM            0x8000000000ULL  /* Share memory space */
#define CLONE_THREAD        0x1000000000ULL  /* Share thread group */
#define CLONE_SIGHAND       0x0200000000ULL  /* Share signal handlers */
#define CLONE_FILES         0x00100000ULL    /* Share file descriptor table */
#define CLONE_FS            0x00200000ULL    /* Share filesystem info */
#define SIGCHLD             17

/* Clone args structure - must match kernel */
struct clone_args {
    uint64_t flags;
    uint64_t stack;
    uint64_t stack_size;
    uint64_t entry;
    uint64_t esignal;
    uint64_t tls;
    uint64_t ctid;
    uint64_t ptid;
};

/* xv6 clone syscall */
extern int clone(struct clone_args *args);

/* Thread stack size */
#define PTHREAD_STACK_SIZE (64 * 1024)  /* 64KB per thread */

/* Thread states */
#define THREAD_UNUSED    0
#define THREAD_RUNNING   1
#define THREAD_EXITED    2
#define THREAD_DETACHED  3

/* Maximum threads */
#define MAX_THREADS 64

/* Thread control block */
struct thread_info {
    volatile int state;
    pthread_t handle;             /* Handle (index + 1) */
    pid_t kernel_tid;             /* Kernel thread/process ID */
    void *stack;                  /* Allocated stack */
    void *(*start_routine)(void *);
    void *arg;
    void *retval;                 /* Return value */
    volatile int exited;          /* Thread has exited */
};

static struct thread_info threads[MAX_THREADS];

/* Simple spinlock for thread table */
static volatile int thread_table_lock = 0;

static void lock_threads(void) {
    while (__sync_lock_test_and_set(&thread_table_lock, 1)) {
        /* spin */
    }
}

static void unlock_threads(void) {
    __sync_lock_release(&thread_table_lock);
}

/* Find a free thread slot and return its index */
static int alloc_thread_slot(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i].state == THREAD_UNUSED) {
            threads[i].state = THREAD_RUNNING;
            threads[i].handle = (pthread_t)(i + 1);  /* Handle is index + 1 */
            return i;
        }
    }
    return -1;
}

/* Find thread by handle */
static struct thread_info *find_thread(pthread_t handle) {
    if (handle == 0 || handle > MAX_THREADS) {
        return NULL;
    }
    int idx = (int)handle - 1;
    if (threads[idx].state == THREAD_UNUSED) {
        return NULL;
    }
    return &threads[idx];
}

/* Find thread by kernel TID */
static struct thread_info *find_thread_by_tid(pid_t tid) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i].state != THREAD_UNUSED && threads[i].kernel_tid == tid) {
            return &threads[i];
        }
    }
    return NULL;
}

/* Thread entry wrapper - called by clone'd thread */
static void thread_entry_wrapper(void) {
    /* Get our thread info - we need to find ourselves by kernel TID */
    pid_t self = getpid();
    
    struct thread_info *ti = find_thread_by_tid(self);
    void *retval = NULL;
    
    if (ti && ti->start_routine) {
        retval = ti->start_routine(ti->arg);
    }
    
    if (ti) {
        ti->retval = retval;
        ti->exited = 1;
        if (ti->state == THREAD_DETACHED) {
            /* Free resources for detached thread */
            if (ti->stack) {
                free(ti->stack);
                ti->stack = NULL;
            }
            ti->state = THREAD_UNUSED;
        } else {
            ti->state = THREAD_EXITED;
        }
    }
    
    _exit(0);
}

/*
 * pthread_create - Create a new thread
 */
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
    (void)attr;
    
    if (!thread || !start_routine) {
        return EINVAL;
    }
    
    lock_threads();
    
    /* Allocate thread slot */
    int idx = alloc_thread_slot();
    if (idx < 0) {
        unlock_threads();
        return EAGAIN;
    }
    
    struct thread_info *ti = &threads[idx];
    
    /* Allocate stack */
    ti->stack = malloc(PTHREAD_STACK_SIZE);
    if (!ti->stack) {
        ti->state = THREAD_UNUSED;
        unlock_threads();
        return ENOMEM;
    }
    
    ti->start_routine = start_routine;
    ti->arg = arg;
    ti->retval = NULL;
    ti->exited = 0;
    ti->kernel_tid = 0;  /* Will be set after clone */
    
    /* Set up clone args for thread creation */
    struct clone_args args;
    memset(&args, 0, sizeof(args));
    args.flags = CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_SIGHAND | SIGCHLD;
    args.stack = (uint64_t)ti->stack;
    args.stack_size = PTHREAD_STACK_SIZE;
    args.entry = (uint64_t)thread_entry_wrapper;
    
    unlock_threads();
    
    /* Create the thread */
    int pid = clone(&args);
    
    if (pid < 0) {
        lock_threads();
        free(ti->stack);
        ti->stack = NULL;
        ti->state = THREAD_UNUSED;
        unlock_threads();
        return -pid;  /* clone returns negative errno */
    }
    
    ti->kernel_tid = (pid_t)pid;
    *thread = ti->handle;
    
    return 0;
}

/*
 * pthread_join - Wait for thread termination
 */
int pthread_join(pthread_t thread, void **retval) {
    struct thread_info *ti = find_thread(thread);
    
    if (!ti) {
        return ESRCH;  /* No such thread */
    }
    
    if (ti->state == THREAD_DETACHED) {
        return EINVAL;  /* Cannot join detached thread */
    }
    
    /* Wait for the thread to exit */
    int status;
    waitpid(ti->kernel_tid, &status, 0);
    
    /* Get return value */
    if (retval) {
        *retval = ti->retval;
    }
    
    /* Free resources */
    lock_threads();
    if (ti->stack) {
        free(ti->stack);
        ti->stack = NULL;
    }
    ti->state = THREAD_UNUSED;
    unlock_threads();
    
    return 0;
}

/*
 * pthread_detach - Detach a thread
 */
int pthread_detach(pthread_t thread) {
    struct thread_info *ti = find_thread(thread);
    
    if (!ti) {
        return ESRCH;
    }
    
    lock_threads();
    if (ti->state == THREAD_EXITED) {
        /* Already exited, just clean up */
        if (ti->stack) {
            free(ti->stack);
            ti->stack = NULL;
        }
        ti->state = THREAD_UNUSED;
    } else {
        ti->state = THREAD_DETACHED;
    }
    unlock_threads();
    
    return 0;
}

/*
 * pthread_self - Get current thread handle
 */
pthread_t pthread_self(void) {
    pid_t tid = getpid();
    struct thread_info *ti = find_thread_by_tid(tid);
    if (ti) {
        return ti->handle;
    }
    /* Main thread - return 0 as special handle */
    return 0;
}

/*
 * pthread_equal - Compare thread handles
 */
int pthread_equal(pthread_t t1, pthread_t t2) {
    return t1 == t2;
}

/*
 * Mutex operations
 * We use atomic operations on the uint32_t mutex value.
 */

#define MUTEX_UNLOCKED    0xFFFFFFFF
#define MUTEX_LOCKED      1
#define MUTEX_FREE        0

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
    (void)attr;
    if (mutex) {
        *mutex = MUTEX_UNLOCKED;
    }
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    if (mutex) {
        *mutex = MUTEX_FREE;
    }
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    if (!mutex) return EINVAL;
    
    while (1) {
        uint32_t val = *mutex;
        if (val == MUTEX_UNLOCKED || val == MUTEX_FREE) {
            if (__sync_bool_compare_and_swap(mutex, val, MUTEX_LOCKED)) {
                return 0;
            }
        } else if (val == MUTEX_LOCKED) {
            while (*mutex == MUTEX_LOCKED) {
                /* spin */
            }
        } else {
            if (__sync_bool_compare_and_swap(mutex, val, MUTEX_LOCKED)) {
                return 0;
            }
        }
    }
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    if (!mutex) return EINVAL;
    
    uint32_t val = *mutex;
    if (val == MUTEX_UNLOCKED || val == MUTEX_FREE) {
        if (__sync_bool_compare_and_swap(mutex, val, MUTEX_LOCKED)) {
            return 0;
        }
    }
    return EBUSY;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    if (!mutex) return EINVAL;
    *mutex = MUTEX_UNLOCKED;
    __sync_synchronize();
    return 0;
}

/*
 * Condition variables - spinning implementation
 */

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
    (void)attr;
    if (cond) {
        *cond = 0xFFFFFFFF;
    }
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
    if (cond) {
        *cond = 0;
    }
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    (void)cond;
    pthread_mutex_unlock(mutex);
    
    struct timespec ts = { 0, 1000000 };  /* 1ms */
    nanosleep(&ts, NULL);
    
    pthread_mutex_lock(mutex);
    return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime) {
    (void)cond;
    (void)abstime;
    pthread_mutex_unlock(mutex);
    
    struct timespec ts = { 0, 1000000 };  /* 1ms */
    nanosleep(&ts, NULL);
    
    pthread_mutex_lock(mutex);
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond) {
    (void)cond;
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
    (void)cond;
    return 0;
}

/*
 * Thread-local storage
 */
#define MAX_TLS_KEYS 64
static void *tls_values[MAX_THREADS + 1][MAX_TLS_KEYS];
static int tls_used[MAX_TLS_KEYS];
static void (*tls_destructors[MAX_TLS_KEYS])(void *);

static int get_thread_index(void) {
    pthread_t self = pthread_self();
    if (self == 0) {
        return MAX_THREADS;
    }
    return (int)self - 1;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
    for (int i = 0; i < MAX_TLS_KEYS; i++) {
        if (!tls_used[i]) {
            tls_used[i] = 1;
            tls_destructors[i] = destructor;
            *key = i;
            return 0;
        }
    }
    return EAGAIN;
}

int pthread_key_delete(pthread_key_t key) {
    if (key >= MAX_TLS_KEYS) return EINVAL;
    tls_used[key] = 0;
    tls_destructors[key] = NULL;
    return 0;
}

int pthread_setspecific(pthread_key_t key, const void *value) {
    if (key >= MAX_TLS_KEYS || !tls_used[key]) return EINVAL;
    int idx = get_thread_index();
    tls_values[idx][key] = (void *)value;
    return 0;
}

void *pthread_getspecific(pthread_key_t key) {
    if (key >= MAX_TLS_KEYS || !tls_used[key]) return NULL;
    int idx = get_thread_index();
    return tls_values[idx][key];
}

/*
 * Once initialization
 */
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
    if (!once_control || !init_routine) return EINVAL;
    
    volatile int *flag = (volatile int *)once_control;
    
    if (__sync_bool_compare_and_swap(flag, 0, 1)) {
        init_routine();
        *flag = 2;
    } else {
        while (*flag == 1) {
            /* spin */
        }
    }
    return 0;
}

/*
 * Attribute functions
 */
int pthread_attr_init(pthread_attr_t *attr) {
    if (attr) {
        memset(attr, 0, sizeof(*attr));
        attr->is_initialized = 1;
        attr->detachstate = PTHREAD_CREATE_JOINABLE;
    }
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr) {
    if (attr) {
        attr->is_initialized = 0;
    }
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
    if (attr) {
        attr->stacksize = (int)stacksize;
    }
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
    (void)attr;
    if (stacksize) *stacksize = PTHREAD_STACK_SIZE;
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
    if (attr) {
        attr->detachstate = detachstate;
    }
    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
    if (detachstate) {
        *detachstate = attr ? attr->detachstate : PTHREAD_CREATE_JOINABLE;
    }
    return 0;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope) {
    if (attr) {
        attr->contentionscope = scope;
    }
    return 0;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *scope) {
    if (scope) {
        *scope = attr ? attr->contentionscope : 0;
    }
    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
    if (attr) {
        attr->stackaddr = stackaddr;
        attr->stacksize = (int)stacksize;
    }
    return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
    if (attr && stackaddr) *stackaddr = attr->stackaddr;
    if (stacksize) *stacksize = attr ? (size_t)attr->stacksize : PTHREAD_STACK_SIZE;
    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    if (attr) {
        memset(attr, 0, sizeof(*attr));
        attr->is_initialized = 1;
    }
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    if (attr) {
        attr->is_initialized = 0;
    }
    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    (void)attr;
    (void)type;
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
    (void)attr;
    if (type) *type = PTHREAD_MUTEX_NORMAL;
    return 0;
}

int pthread_condattr_init(pthread_condattr_t *attr) {
    if (attr) {
        memset(attr, 0, sizeof(*attr));
        attr->is_initialized = 1;
    }
    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr) {
    if (attr) {
        attr->is_initialized = 0;
    }
    return 0;
}

/*
 * Cancellation - not supported
 */
int pthread_cancel(pthread_t thread) {
    (void)thread;
    return ENOTSUP;
}

int pthread_setcancelstate(int state, int *oldstate) {
    (void)state;
    if (oldstate) *oldstate = PTHREAD_CANCEL_DISABLE;
    return 0;
}

int pthread_setcanceltype(int type, int *oldtype) {
    (void)type;
    if (oldtype) *oldtype = PTHREAD_CANCEL_DEFERRED;
    return 0;
}

void pthread_testcancel(void) {
    /* No-op */
}

/*
 * pthread_exit - Exit current thread
 */
void pthread_exit(void *retval) {
    pthread_t self = pthread_self();
    struct thread_info *ti = find_thread(self);
    
    if (ti) {
        ti->retval = retval;
        ti->exited = 1;
        ti->state = THREAD_EXITED;
    }
    
    _exit(0);
}

/*
 * Additional stubs
 */
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
    (void)prepare;
    (void)parent;
    (void)child;
    return 0;
}

int pthread_sigmask(int how, const void *set, void *oldset) {
    (void)how;
    (void)set;
    (void)oldset;
    return 0;
}

void pthread_yield(void) {
    struct timespec ts = { 0, 1000 };
    nanosleep(&ts, NULL);
}
