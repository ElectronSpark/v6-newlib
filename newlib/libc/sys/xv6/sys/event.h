/**
 * @file sys/event.h
 * @brief BSD kqueue event notification interface for xv6/newlib
 *
 * Provides the user-visible ABI: struct kevent, filter and flag constants,
 * and function declarations for kqueue(), kevent_register(), kevent_wait().
 */

#ifndef _SYS_EVENT_H
#define _SYS_EVENT_H

#include <sys/types.h>
#include <stdint.h>
#include <time.h>

/*
 * Event filters (negative values, BSD convention)
 */
#define EVFILT_READ     (-1)
#define EVFILT_WRITE    (-2)
#define EVFILT_TIMER    (-3)
#define EVFILT_SIGNAL   (-4)
#define EVFILT_PROC     (-5)
#define EVFILT_VNODE    (-6)

/*
 * Event flags (bitfield in kevent.flags)
 */
#define EV_ADD          0x0001  /* add event to kqueue */
#define EV_DELETE       0x0002  /* delete event from kqueue */
#define EV_ENABLE       0x0004  /* enable event */
#define EV_DISABLE      0x0008  /* disable event */
#define EV_ONESHOT      0x0010  /* only report one occurrence */
#define EV_CLEAR        0x0020  /* clear event state after retrieval */
#define EV_EOF          0x8000  /* EOF detected */
#define EV_ERROR        0x4000  /* error, data contains errno */

/*
 * EVFILT_PROC filter-specific flags (fflags)
 */
#define NOTE_EXIT       0x80000000
#define NOTE_FORK       0x40000000
#define NOTE_EXEC       0x20000000
#define NOTE_TRACK      0x00000001
#define NOTE_CHILD      0x00000004
#define NOTE_TRACKERR   0x00000002
#define NOTE_PCTRLMASK  0xf0000000
#define NOTE_PDATAMASK  0x000fffff

/*
 * EVFILT_VNODE filter-specific flags (fflags)
 */
#define NOTE_DELETE     0x00000001
#define NOTE_WRITE      0x00000002
#define NOTE_EXTEND     0x00000004
#define NOTE_ATTRIB     0x00000008
#define NOTE_LINK       0x00000010
#define NOTE_RENAME     0x00000020
#define NOTE_REVOKE     0x00000040

/*
 * struct kevent - user-space event structure (the ABI)
 *
 * BSD-compatible layout.  Must match the kernel's struct kevent exactly.
 */
struct kevent {
    uint64_t ident;         /* identifier for this event (fd, signal, pid) */
    int16_t  filter;        /* filter for event (EVFILT_*) */
    uint16_t flags;         /* action flags (EV_*) */
    uint32_t fflags;        /* filter-specific flags (NOTE_*) */
    int64_t  data;          /* filter-specific data */
    uint64_t udata;         /* opaque user data */
};

/*
 * Convenience macro à la BSD: initialise a struct kevent in-place.
 */
#define EV_SET(kevp, a, b, c, d, e, f) do { \
    struct kevent *__kevp = (kevp);          \
    __kevp->ident  = (a);                   \
    __kevp->filter = (b);                   \
    __kevp->flags  = (c);                   \
    __kevp->fflags = (d);                   \
    __kevp->data   = (e);                   \
    __kevp->udata  = (f);                   \
} while (0)

__BEGIN_DECLS

/**
 * Create a new kqueue file descriptor.
 * @return kqueue fd on success, -1 on error (errno set).
 */
int kqueue(void);

/**
 * Register, modify, or delete events on a kqueue.
 * @param kqfd       kqueue file descriptor
 * @param changelist array of kevent changes
 * @param nchanges   number of entries in changelist
 * @return 0 on success, -1 on error (errno set).
 */
int kevent_register(int kqfd, struct kevent *changelist, int nchanges);

/**
 * Wait for events on a kqueue.
 * @param kqfd       kqueue file descriptor
 * @param eventlist  output buffer for triggered events
 * @param nevents    maximum number of events to return
 * @param timeout_ms timeout: -1 = block, 0 = poll, >0 = milliseconds
 * @return number of ready events, or -1 on error (errno set).
 */
int kevent_wait(int kqfd, struct kevent *eventlist, int nevents,
                int timeout_ms);

/**
 * BSD-compatible combined register-and-wait.
 * @param kq         kqueue file descriptor
 * @param changelist array of kevent changes (may be NULL if nchanges == 0)
 * @param nchanges   number of entries in changelist
 * @param eventlist  output buffer for triggered events (may be NULL if nevents == 0)
 * @param nevents    maximum number of events to return
 * @param timeout    timeout (NULL = block, tv_sec==0 && tv_nsec==0 = poll)
 * @return number of ready events, or -1 on error (errno set).
 */
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents,
           const struct timespec *timeout);

__END_DECLS

#endif /* _SYS_EVENT_H */
