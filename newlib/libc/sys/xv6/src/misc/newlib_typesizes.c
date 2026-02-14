#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
    printf("=== newlib type sizes (bytes) ===\n");
    printf("dev_t    : %u\n", (unsigned)sizeof(dev_t));
    printf("ino_t    : %u\n", (unsigned)sizeof(ino_t));
    printf("nlink_t  : %u\n", (unsigned)sizeof(nlink_t));
    printf("off_t    : %u\n", (unsigned)sizeof(off_t));
    printf("uid_t    : %u\n", (unsigned)sizeof(uid_t));
    printf("gid_t    : %u\n", (unsigned)sizeof(gid_t));
    printf("mode_t   : %u\n", (unsigned)sizeof(mode_t));
    printf("id_t     : %u\n", (unsigned)sizeof(id_t));
    printf("size_t   : %u\n", (unsigned)sizeof(size_t));
    printf("ssize_t  : %u\n", (unsigned)sizeof(ssize_t));
    return 0;
}
