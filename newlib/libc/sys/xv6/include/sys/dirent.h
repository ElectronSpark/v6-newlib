/*
 * sys/dirent.h - Directory entry structures for xv6 with newlib
 * 
 * This header provides the dirent structures and DIR type that
 * newlib's <dirent.h> expects but doesn't provide for bare-metal targets.
 */

#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#include <sys/types.h>

/* File type flags for d_type */
#define DT_UNKNOWN   0
#define DT_FIFO      1
#define DT_CHR       2
#define DT_DIR       4
#define DT_BLK       6
#define DT_REG       8
#define DT_LNK      10
#define DT_SOCK     12
#define DT_WHT      14

/* Directory entry structure - matches Linux's dirent64 layout */
struct dirent {
    ino_t       d_ino;       /* Inode number */
    off_t       d_off;       /* Offset to next structure */
    unsigned short d_reclen; /* Size of this dirent */
    unsigned char  d_type;   /* File type */
    char        d_name[256]; /* Filename (null-terminated) */
};

/* Internal buffer size for directory reading */
#define _DIR_BUF_SIZE 1024

/* DIR structure for directory stream */
typedef struct {
    int             dd_fd;           /* File descriptor */
    int             dd_loc;          /* Current position in buffer */
    int             dd_size;         /* Amount of valid data in buffer */
    struct dirent   dd_ent;          /* Current entry (returned by readdir) */
    char            dd_buf[_DIR_BUF_SIZE]; /* Buffer for getdents */
} DIR;

#endif /* _SYS_DIRENT_H */
