/*
 * sys/utsname.h - System identification for xv6
 */

#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

#ifdef __cplusplus
extern "C" {
#endif

#define _UTSNAME_LENGTH 65

struct utsname {
    char sysname[_UTSNAME_LENGTH];    /* OS name */
    char nodename[_UTSNAME_LENGTH];   /* Network node name (hostname) */
    char release[_UTSNAME_LENGTH];    /* OS release */
    char version[_UTSNAME_LENGTH];    /* OS version */
    char machine[_UTSNAME_LENGTH];    /* Hardware identifier */
};

/* uname() - get system information */
int uname(struct utsname *buf);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UTSNAME_H */
