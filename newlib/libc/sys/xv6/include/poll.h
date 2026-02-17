/**
 * @file poll.h
 * @brief poll() definitions for xv6/newlib
 */

#ifndef _POLL_H
#define _POLL_H

/* Event types for struct pollfd.events / revents */
#ifndef POLLIN
#define POLLIN     0x0001   /* There is data to read */
#define POLLPRI    0x0002   /* Urgent data to read */
#define POLLOUT    0x0004   /* Writing now will not block */
#define POLLERR    0x0008   /* Error condition (revents only) */
#define POLLHUP    0x0010   /* Hung up (revents only) */
#define POLLNVAL   0x0020   /* Invalid fd (revents only) */
#define POLLRDNORM 0x0040   /* Normal data may be read */
#define POLLRDBAND 0x0080   /* Priority data may be read */
#define POLLWRNORM 0x0100   /* Writing now will not block */
#define POLLWRBAND 0x0200   /* Priority data may be written */

struct pollfd {
    int   fd;       /* file descriptor */
    short events;   /* requested events */
    short revents;  /* returned events */
};
#endif /* POLLIN */

typedef unsigned long nfds_t;

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif /* _POLL_H */
