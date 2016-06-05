#define _GNU_SOURCE

#include "common.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Queried system for this number.
#define BLOCK_SIZE 4096
#define MAX_FS_BITS 31
#define MIN_FS_BITS MAX_FS_BITS-10

// WARNING: These functions specifically are optimized! 
//double measure_fc_access(int, int, bool) __attribute__((optimize("Ofast")));


double measure_fc_access(int fd, int num_blocks, bool is_random_access) {
    struct timespec start, end;
    char* buf = NULL;
    int indices[num_blocks];

    for (int i = 0; i < num_blocks; ++i) {
        indices[i] = i;
    }

    if (is_random_access) {
        // Fisher-Yates shuffle.
        for (int i = num_blocks-1; i > 0; --i) {
            int j = rand() % i;

            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
    }

    // Align for O_DIRECT.
    posix_memalign((void**)&buf, BLOCK_SIZE, BLOCK_SIZE);
    if (buf == NULL) {
        puts("Error allocating aligned mem.\n");
        exit(1);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_blocks; ++i) {
        pread(fd, buf, BLOCK_SIZE, indices[i]*BLOCK_SIZE);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // Delete aligned mem.
    free(buf);

    return timespec_diff_to_nsecs(start, end) - num_blocks*FOR_LOOP_OVERHEAD - GET_TIME_OVERHEAD;
}

void write_to_disk(const char *filename, int num_blocks) {
    char buf[1024];

    // Use the system PRNG.
    snprintf(buf, 1024, "dd if=/dev/urandom of=test.file count=%d bs=4096 status=none", num_blocks);
    assert(system(buf) == 0);
}

void print_for(int num_bytes, bool is_random_access) {
    int num_blocks = num_bytes / BLOCK_SIZE;

    // First, write a file to disk (we're measuring block from disk time).
    write_to_disk("test.file", num_blocks);

    // Flush caches. We shouldn't be using them, but this enforces it.
    assert(system("sync; echo 1 >|/proc/sys/vm/drop_caches") == 0);

    // Open the test file.
    int fd = open("test.file", O_SYNC | O_DIRECT | O_RDONLY);
    assert(fd != -1 && "open() failed.");

    // Measure the time loading it.
    double read_time = measure_fc_access(fd, num_blocks, is_random_access);

    printf("%d MB: %.2f us/block\n",
           num_blocks * BLOCK_SIZE / 1024 / 1024,
           read_time / 1000 / num_blocks);

    // Close the file.
    close(fd);

    // Delete the file.
    unlink("test.file");
}

int main(int argc, char **argv) {
    assert(init_test() == 0 && "Could not initialize.");

    puts("Random:");

    for (size_t fs_bits = MIN_FS_BITS; fs_bits < MAX_FS_BITS; ++fs_bits) {
        print_for(1<<fs_bits, true);
    }

    puts("\nSequential:");

    for (size_t fs_bits = MIN_FS_BITS; fs_bits < MAX_FS_BITS; ++fs_bits) {
        print_for(1<<fs_bits, false);
    }

    return 0;
}
