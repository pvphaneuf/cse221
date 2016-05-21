#include "common.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Queried system for this number.
#define PAGE_SIZE 4096

// WARNING: These functions specifically are optimized! 
double measure_fc_access(int *, int) __attribute__((optimize("Ofast")));


double measure_fc_access(int *mem, int num_pages) {
    struct timespec start, end;
    volatile int dummy;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_pages; i += PAGE_SIZE) {
        dummy += mem[i];
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return timespec_diff_to_nsecs(start, end);
}

void write_to_disk(const char *filename, int num_pages) {
    FILE *f = fopen(filename, "w");
    assert(f && "unable to open file.");

    for(int i = 0; i < PAGE_SIZE * num_pages; ++i) {
        assert(fputc(i, f) != EOF && "error writing file.");
    }

    fclose(f);
}

void print_for(int num_bytes) {
    int num_pages = num_bytes / 4096;
    // First, write a file to disk (we're measuring page from disk time).
    write_to_disk("test.file", num_pages);

    // Then, open it with mmap.
    int fd = open("test.file", O_RDONLY);
    int *mem = mmap(NULL, PAGE_SIZE * num_pages, PROT_READ,
        MAP_PRIVATE, fd, 0);
    assert(mem != (void *)-1 && "mmap failed.");

    // Populate the file cache by touching each page a few times.
    for (int i = 0; i < 3; ++i) measure_fc_access(mem, num_pages);

    // Measure the time loading it.
    double read_time = 0;
    for (int i = 0; i < 3; ++i) {
        read_time += measure_fc_access(mem, num_pages);
    }
    read_time /= 3;

    printf("%d pages: average page read time: %f cycles/page\n", num_pages, read_time / 0.833);

    // Close the file.
    assert(munmap(mem, PAGE_SIZE * num_pages) == 0 && "munmap failed.");
    close(fd);
}

int main() {
    assert(init_test() == 0 && "Could not initialize.");

    for (size_t fs_bits = 12; fs_bits < 28; ++fs_bits) {
        print_for(1<<fs_bits);
    }

    return 0;
}
