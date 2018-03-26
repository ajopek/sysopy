#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    size_t blocks_num = atoi(argv[1]);
    size_t block_size = atoi(argv[2]);
    size_t total_size = 0;
    char* curr_block;
    for (int i = 0; i < blocks_num; ++i) {
        printf("Allocating %zu MiB of memory, total size: %zu MiB.\n", block_size, total_size + block_size);
        curr_block = malloc(block_size * 1024 * 1024);
        memset(curr_block, 1, block_size * 1024 * 1024);
        total_size += block_size;
    }

    return 0;
}
