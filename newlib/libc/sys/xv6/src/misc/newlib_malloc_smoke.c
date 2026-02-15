#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int check_fill(unsigned char *buf, size_t len, unsigned char pattern)
{
    for (size_t i = 0; i < len; i++) {
        if (buf[i] != pattern) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    printf("newlib malloc smoke start\n");

    size_t sizes[] = {64, 256, 1024, 4096, 16384, 65536};
    unsigned char patterns[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    enum { N = sizeof(sizes) / sizeof(sizes[0]) };

    void *blocks[N];
    for (int i = 0; i < N; i++) {
        blocks[i] = malloc(sizes[i]);
        if (blocks[i] == NULL) {
            printf("malloc failed at size=%lu\n", (unsigned long)sizes[i]);
            return 1;
        }
        memset(blocks[i], patterns[i], sizes[i]);
    }

    for (int i = 0; i < N; i++) {
        if (!check_fill((unsigned char *)blocks[i], sizes[i], patterns[i])) {
            printf("pattern mismatch at block=%d size=%lu\n", i, (unsigned long)sizes[i]);
            return 2;
        }
    }

    for (int i = N - 1; i >= 0; i--) {
        free(blocks[i]);
    }

    void *reuse = malloc(512);
    if (reuse == NULL) {
        printf("malloc failed after free/reuse phase\n");
        return 3;
    }
    memset(reuse, 0xA5, 512);
    free(reuse);

    enum { BIG_CHUNK = 32768, MAX_BIG_BLOCKS = 64 };
    void *big_blocks[MAX_BIG_BLOCKS];
    int big_count = 0;
    size_t total = 0;

    for (int i = 0; i < MAX_BIG_BLOCKS; i++) {
        big_blocks[i] = malloc(BIG_CHUNK);
        if (big_blocks[i] == NULL) {
            break;
        }
        memset(big_blocks[i], 0x7C, BIG_CHUNK);
        total += BIG_CHUNK;
        big_count++;
    }

    if (big_count == 0) {
        printf("large allocation phase failed immediately\n");
        return 4;
    }

    for (int i = 0; i < big_count; i++) {
        if (!check_fill((unsigned char *)big_blocks[i], BIG_CHUNK, 0x7C)) {
            printf("large block pattern mismatch at block=%d\n", i);
            return 5;
        }
    }

    printf("large allocation phase: %d blocks, total=%lu bytes\n",
           big_count, (unsigned long)total);

    for (int i = big_count - 1; i >= 0; i--) {
        free(big_blocks[i]);
    }

    printf("newlib malloc smoke: OK\n");
    return 0;
}
