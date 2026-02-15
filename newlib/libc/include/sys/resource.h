#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>

#define	RUSAGE_SELF	0		/* calling process */
#define	RUSAGE_CHILDREN	-1		/* terminated child processes */
#if __GNU_VISIBLE
#define	RUSAGE_THREAD	1
#endif

struct rusage {
  	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
};

typedef unsigned long rlim_t;

struct rlimit {
  rlim_t rlim_cur;
  rlim_t rlim_max;
};

#ifndef RLIM_INFINITY
#define RLIM_INFINITY ((rlim_t)-1)
#endif

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

int	getrusage (int, struct rusage*);
int getrlimit (int, struct rlimit *);
int setrlimit (int, const struct rlimit *);
int prlimit (pid_t, int, const struct rlimit *, struct rlimit *);

#ifdef __cplusplus
}
#endif
#endif /* !_SYS_RESOURCE_H_ */

