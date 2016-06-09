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
double measure_pf_access(int *, int) __attribute__((optimize("O2")));


double measure_pf_access(int *mem, int num_pages) {
    struct timespec start, end;
    volatile int dummy;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_pages; i += PAGE_SIZE) {
        dummy += mem[i];
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // Don't need to subtract overhead--we're looking at the difference in
    // times.
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

void print_pages(int num_pages) {
    double cold_time, hot_time[30], avg, std;

    // First, write a file to disk (we're measuring page from disk time).
    write_to_disk("test.file", num_pages);

    // Then, open it with mmap.
    int fd = open("test.file", O_RDONLY);
    int *mem = mmap(NULL, PAGE_SIZE * num_pages, PROT_READ,
        MAP_PRIVATE, fd, 0);
    assert(mem != (void *)-1 && "mmap failed.");

    // Measure the time loading it cold.
    cold_time = measure_pf_access(mem, num_pages);

    // Warm-up.
    for(int i = 0; i < 15; ++i) measure_pf_access(mem, num_pages);

    // Measure the time loading it hot.
    for (int i = 0; i < 30; ++i) {
        hot_time[i] = measure_pf_access(mem, num_pages);
    }
    avg = get_mean(hot_time, 30);
    std = get_mean(hot_time, 30);

    printf("%d pages: page fault timing: %fK cycles/page, std %f\n", num_pages, (cold_time-avg) / 0.833 / 1000, std / 0.833 / 0.833 / 1000 / 1000);

    // Close the file.
    assert(munmap(mem, PAGE_SIZE * num_pages) == 0 && "munmap failed.");
    close(fd);
}

int main() {
    assert(init_test() == 0 && "Could not initialize.");

    print_pages(1024);
    print_pages(2048);
    print_pages(4096);
    print_pages(8192);

    return 0;
}
