/* errno is not a global variable, because that would make using it
   non-reentrant.  Instead, its address is returned by the function
   __errno.  */

#ifndef _SYS_ERRNO_H_
#ifdef __cplusplus
extern "C" {
#endif
#define _SYS_ERRNO_H_

#include <sys/reent.h>

#ifdef _REENT_THREAD_LOCAL
#define errno (_tls_errno)
#else /* _REENT_THREAD_LOCAL */

#ifndef _REENT_ONLY
#define errno (*__errno())
extern int *__errno (void);
#endif

#endif /* _REENT_THREAD_LOCAL */

/* Please don't use these variables directly.
   Use strerror instead. */
extern __IMPORT const char * const _sys_errlist[];
extern __IMPORT int _sys_nerr;
#ifdef __CYGWIN__
extern __IMPORT const char * const sys_errlist[];
extern __IMPORT int sys_nerr;
extern __IMPORT char *program_invocation_name;
extern __IMPORT char *program_invocation_short_name;
#endif

#define __errno_r(ptr) _REENT_ERRNO(ptr)

#define	EPERM 1		/* Not owner */
#define	ENOENT 2	/* No such file or directory */
#define	ESRCH 3		/* No such process */
#define	EINTR 4		/* Interrupted system call */
#define	EIO 5		/* I/O error */
#define	ENXIO 6		/* No such device or address */
#define	E2BIG 7		/* Arg list too long */
#define	ENOEXEC 8	/* Exec format error */
#define	EBADF 9		/* Bad file number */
#define	ECHILD 10	/* No children */
#define	EAGAIN 11	/* No more processes */
#define	ENOMEM 12	/* Not enough space */
#define	EACCES 13	/* Permission denied */
#define	EFAULT 14	/* Bad address */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define	ENOTBLK 15	/* Block device required */
#endif
#define	EBUSY 16	/* Device or resource busy */
#define	EEXIST 17	/* File exists */
#define	EXDEV 18	/* Cross-device link */
#define	ENODEV 19	/* No such device */
#define	ENOTDIR 20	/* Not a directory */
#define	EISDIR 21	/* Is a directory */
#define	EINVAL 22	/* Invalid argument */
#define	ENFILE 23	/* Too many open files in system */
#define	EMFILE 24	/* File descriptor value too large */
#define	ENOTTY 25	/* Not a character device */
#define	ETXTBSY 26	/* Text file busy */
#define	EFBIG 27	/* File too large */
#define	ENOSPC 28	/* No space left on device */
#define	ESPIPE 29	/* Illegal seek */
#define	EROFS 30	/* Read-only file system */
#define	EMLINK 31	/* Too many links */
#define	EPIPE 32	/* Broken pipe */
#define	EDOM 33		/* Mathematics argument out of domain of function */
#define	ERANGE 34	/* Result too large */

/* Linux-compatible errno values (matching xv6 kernel) */
#define	EDEADLK 35	/* Deadlock */
#define	ENAMETOOLONG 36	/* File or path name too long */
#define	ENOLCK 37	/* No lock */
#define	ENOSYS 38	/* Function not implemented */
#define	ENOTEMPTY 39	/* Directory not empty */
#define	ELOOP 40	/* Too many symbolic links */
#define	EWOULDBLOCK EAGAIN	/* Operation would block */
#define	ENOMSG 42	/* No message of desired type */
#define	EIDRM 43	/* Identifier removed */
#define	ECHRNG 44	/* Channel number out of range */
#define	EL2NSYNC 45	/* Level 2 not synchronized */
#define	EL3HLT 46	/* Level 3 halted */
#define	EL3RST 47	/* Level 3 reset */
#define	ELNRNG 48	/* Link number out of range */
#define	EUNATCH 49	/* Protocol driver not attached */
#define	ENOCSI 50	/* No CSI structure available */
#define	EL2HLT 51	/* Level 2 halted */
#define	EBADE 52	/* Invalid exchange */
#define	EBADR 53	/* Invalid request descriptor */
#define	EXFULL 54	/* Exchange full */
#define	ENOANO 55	/* No anode */
#define	EBADRQC 56	/* Invalid request code */
#define	EBADSLT 57	/* Invalid slot */
#define	EDEADLOCK EDEADLK	/* File locking deadlock error */
#define	EBFONT 59	/* Bad font file fmt */
#define	ENOSTR 60	/* Not a stream */
#define	ENODATA 61	/* No data (for no delay io) */
#define	ETIME 62	/* Stream ioctl timeout */
#define	ENOSR 63	/* No stream resources */
#define	ENONET 64	/* Machine is not on the network */
#define	ENOPKG 65	/* Package not installed */
#define	EREMOTE 66	/* The object is remote */
#define	ENOLINK 67	/* Virtual circuit is gone */
#define	EADV 68		/* Advertise error */
#define	ESRMNT 69	/* Srmount error */
#define	ECOMM 70	/* Communication error on send */
#define	EPROTO 71	/* Protocol error */
#define	EMULTIHOP 72	/* Multihop attempted */
#define	EDOTDOT 73	/* Cross mount point */
#define	EBADMSG 74	/* Bad message */
#define	EOVERFLOW 75	/* Value too large for defined data type */
#define	ENOTUNIQ 76	/* Given log. name not unique */
#define	EBADFD 77	/* f.d. invalid for this operation */
#define	EREMCHG 78	/* Remote address changed */
#define	ELIBACC 79	/* Can't access a needed shared lib */
#define	ELIBBAD 80	/* Accessing a corrupted shared lib */
#define	ELIBSCN 81	/* .lib section in a.out corrupted */
#define	ELIBMAX 82	/* Attempting to link in too many libs */
#define	ELIBEXEC 83	/* Attempting to exec a shared library */
#define	EILSEQ 84	/* Illegal byte sequence */
#define	ERESTART 85	/* Interrupted system call should be restarted */
#define	ESTRPIPE 86	/* Streams pipe error */
#define	EUSERS 87	/* Too many users */
#define	ENOTSOCK 88	/* Socket operation on non-socket */
#define	EDESTADDRREQ 89	/* Destination address required */
#define	EMSGSIZE 90	/* Message too long */
#define	EPROTOTYPE 91	/* Protocol wrong type for socket */
#define	ENOPROTOOPT 92	/* Protocol not available */
#define	EPROTONOSUPPORT 93	/* Unknown protocol */
#define	ESOCKTNOSUPPORT 94	/* Socket type not supported */
#define	EOPNOTSUPP 95	/* Operation not supported on socket */
#define	ENOTSUP EOPNOTSUPP	/* Not supported */
#define	EPFNOSUPPORT 96	/* Protocol family not supported */
#define	EAFNOSUPPORT 97	/* Address family not supported by protocol family */
#define	EADDRINUSE 98	/* Address already in use */
#define	EADDRNOTAVAIL 99	/* Address not available */
#define	ENETDOWN 100	/* Network interface is not configured */
#define	ENETUNREACH 101	/* Network is unreachable */
#define	ENETRESET 102	/* Connection aborted by network */
#define	ECONNABORTED 103	/* Software caused connection abort */
#define	ECONNRESET 104	/* Connection reset by peer */
#define	ENOBUFS 105	/* No buffer space available */
#define	EISCONN 106	/* Socket is already connected */
#define	ENOTCONN 107	/* Socket is not connected */
#define	ESHUTDOWN 108	/* Can't send after socket shutdown */
#define	ETOOMANYREFS 109	/* Too many references */
#define	ETIMEDOUT 110	/* Connection timed out */
#define	ECONNREFUSED 111	/* Connection refused */
#define	EHOSTDOWN 112	/* Host is down */
#define	EHOSTUNREACH 113	/* Host is unreachable */
#define	EALREADY 114	/* Socket already connected */
#define	EINPROGRESS 115	/* Connection already in progress */
#define	ESTALE 116	/* Stale file handle */
#define	EUCLEAN 117	/* Structure needs cleaning */
#define	ENOTNAM 118	/* Not a XENIX named type file */
#define	ENAVAIL 119	/* No XENIX semaphores available */
#define	EISNAM 120	/* Is a named type file */
#define	EREMOTEIO 121	/* Remote I/O error */
#define	EDQUOT 122	/* Disk quota exceeded */
#define	ENOMEDIUM 123	/* No medium (in tape drive) */
#define	EMEDIUMTYPE 124	/* Wrong medium type */
#define	ECANCELED 125	/* Operation canceled */
#define	ENOKEY 126	/* Required key not available */
#define	EKEYEXPIRED 127	/* Key has expired */
#define	EKEYREVOKED 128	/* Key has been revoked */
#define	EKEYREJECTED 129	/* Key was rejected by service */
#define	EOWNERDEAD 130	/* Previous owner died */
#define	ENOTRECOVERABLE 131	/* State not recoverable */
#define	ERFKILL 132	/* Operation not possible due to RF-kill */
#define	EHWPOISON 133	/* Memory page has hardware error */

/* Newlib compatibility aliases */
#define	EFTYPE 79	/* Inappropriate file type or format (alias for ELIBACC) */

#define __ELASTERROR 2000	/* Users can add values starting here */

#ifdef __cplusplus
}
#endif
#endif /* _SYS_ERRNO_H */
