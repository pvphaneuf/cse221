#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Queried system for this number.
#define PAGE_SIZE 4096

#define MEGA (1024 * 1024)
#define GIGA (1024 * 1024 * 1024)

typedef long long word;

// WARNING: These functions specifically are optimized! 
double measure_wrmem(word *, int) __attribute__((optimize("O2")));
double measure_rdmem(word *, int) __attribute__((optimize("O2")));

double measure_wrmem(word *mem, int size) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    // This should be grabbed and optimized by the compiler for max bw.
    for (int i = 0; i < size; ++i) {
        mem[i] = i;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // Return the bytes per ns.
    // Not accounting for FOR loop overhead since we're expecting compiler optimization.
    return sizeof(word) * size / \
        (timespec_diff_to_nsecs(start, end) - GET_TIME_OVERHEAD);
}

double measure_rdmem(word *mem, int size) {
    struct timespec start, end;
    volatile int dummy;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    // This should be grabbed and optimized by the compiler for max bw.
    for (int i = 0; i < size; ++i) {
        dummy += mem[i];
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // Return the bytes per ns.
    // Not accounting for FOR loop overhead since we're expecting compiler optimization.
    return sizeof(word) * size / \
        (timespec_diff_to_nsecs(start, end) - GET_TIME_OVERHEAD);
}

void print_rdwr(int pages) {
    int num_words = pages * PAGE_SIZE;
    word *mem = malloc(sizeof(word) * num_words);

    printf("For %d MB:\n", num_words * sizeof(word) / MEGA);

    for (int i = 0; i < 15; ++i) measure_wrmem(mem, num_words);  // warmup runs to population branch prediction buffer, and fault in pages.
    printf("\twrite MB/s: %f\n", measure_wrmem(mem, num_words) * BILLION / MEGA);

    for (int i = 0; i < 15; ++i) measure_rdmem(mem, num_words);  // warmup runs to population branch prediction buffer, and fault in pages.
    printf("\tread MB/s:  %f\n", measure_rdmem(mem, num_words) * BILLION / MEGA);

    free(mem);
}

int main() {
    assert(init_test() == 0 && "Could not initialize.");

    print_rdwr(1024);
    print_rdwr(2048);
    print_rdwr(8192);

    return 0;
}
