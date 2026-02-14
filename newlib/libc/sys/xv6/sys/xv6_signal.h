/*
 * xv6_signal.h - Signal extensions for xv6
 * 
 * Provides additional signal flags and definitions
 * that may not be in newlib's signal.h
 */

#ifndef _XV6_SIGNAL_H
#define _XV6_SIGNAL_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Signal action flags */
#ifndef SA_ONSTACK
#define SA_ONSTACK      0x08000000  /* Use alternate signal stack */
#endif

#ifndef SA_RESTART
#define SA_RESTART      0x10000000  /* Restart syscall on signal return */
#endif

#ifndef SA_NODEFER
#define SA_NODEFER      0x40000000  /* Don't block signal while handling */
#endif

#ifndef SA_RESETHAND
#define SA_RESETHAND    0x80000000  /* Reset to SIG_DFL on entry */
#endif

/* Alternate signal stack constants */
#ifndef MINSIGSTKSZ
#define MINSIGSTKSZ     2048
#endif

#ifndef SIGSTKSZ
#define SIGSTKSZ        8192
#endif

#ifndef SS_ONSTACK
#define SS_ONSTACK      1
#endif

#ifndef SS_DISABLE
#define SS_DISABLE      2
#endif

#ifdef __cplusplus
}
#endif

#endif /* _XV6_SIGNAL_H */
