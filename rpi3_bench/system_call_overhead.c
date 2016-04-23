//#define _GNU_SOURCE
#include <sys/syscall.h>
#include <unistd.h> // getpid()
#include <time.h>   // timespec
#include <stdio.h>

#include "common.h"


unsigned int const TEST_COUNT = 10000;


int system_call_test(void) {
    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (unsigned int result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++) {
        syscall(SYS_gettid);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
    total_time = total_time - GET_TIME_OVERHEAD;
    double average_time = total_time / (double)TEST_COUNT;

    printf("getpid syscall latency: %f ns\n", average_time);

    return 0;
}


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        return -1;
    }

    system_call_test();

    return 0;
}