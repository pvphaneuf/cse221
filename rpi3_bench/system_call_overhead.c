//#define _GNU_SOURCE
#include <sys/syscall.h>
#include <unistd.h> // getpid()
#include <time.h>   // timespec
#include <stdio.h>

#include "common.h"


const unsigned int AVG_TEST_COUNT = 10000;

const unsigned int SUBSEQUENT_CALLS_TEST_COUNT = 10;


void compare_first_to_subsequent_syscalls(void) {
    struct timespec start, stop;

    for (unsigned int iteration_count = 0; iteration_count < SUBSEQUENT_CALLS_TEST_COUNT; iteration_count++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        syscall(SYS_gettid);
        clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

        double latency = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
        latency = latency - GET_TIME_OVERHEAD;
        printf("syscall(SYS_gettid) call count %u latency = %f ns\n", iteration_count, latency);
    }
    printf("\n");
}


void print_avg_syscall(void) {
    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (unsigned int result_array_idx = 0; result_array_idx < AVG_TEST_COUNT; result_array_idx++) {
        syscall(SYS_gettid);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
    total_time = total_time - GET_TIME_OVERHEAD - (FOR_LOOP_OVERHEAD * AVG_TEST_COUNT);

    double average_time = total_time / (double)AVG_TEST_COUNT;

    printf("syscall(SYS_gettid) syscall average latency: %f ns\n\n", average_time);
}


void system_call_test(void) {

    // This part of the test must be before getting the average syscall
    // since we want to catch the difference in time for the first syscall
    // and all subsequent.
    compare_first_to_subsequent_syscalls();

    print_avg_syscall();
}


void compare_first_to_subsequent_wrapper_call(void) {
    struct timespec start, stop;

    for (unsigned int iteration_count = 0; iteration_count < SUBSEQUENT_CALLS_TEST_COUNT; iteration_count++) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        getpid();
        clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

        double latency = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
        latency = latency - GET_TIME_OVERHEAD;
        printf("system call wrapper getpid() call count %u latency = %f ns\n", iteration_count, latency);
    }
    printf("\n");
}


void print_average_syscall(void) {
    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (unsigned int iteration_count = 0; iteration_count < AVG_TEST_COUNT; iteration_count++) {
        getpid();
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
    total_time = total_time - GET_TIME_OVERHEAD - (FOR_LOOP_OVERHEAD * AVG_TEST_COUNT);

    double average_time = total_time / (double)AVG_TEST_COUNT;

    printf("getpid syscall wrapper average latency: %f ns\n\n", average_time);
}


void system_call_wrapper_test(void) {

    // This part of the test must be before getting the average syscall
    // since we want to catch the difference in time for the first syscall
    // and all subsequent.
    compare_first_to_subsequent_wrapper_call();

    print_average_syscall();
}


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        return -1;
    }

    system_call_test();

    system_call_wrapper_test();

    return 0;
}