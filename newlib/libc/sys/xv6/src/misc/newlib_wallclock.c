#include <stdio.h>
#include <sys/time.h>

int main(void) {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0) {
        perror("gettimeofday");
        return 1;
    }

    printf("newlib wallclock: %ld.%06ld\n", (long)tv.tv_sec, (long)tv.tv_usec);
    return 0;
}
