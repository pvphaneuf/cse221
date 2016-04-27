#include <time.h>
#include <stdio.h>

#include "common.h"


int test_iteration_count_1(void)
{
    struct timespec start, stop;
    unsigned int total_time = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < 1; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / 1.0;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, 1);

    return 0;
}


int test_iteration_count_10(void)
{
    struct timespec start, stop;
    unsigned int total_time = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < 10; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / 10.0;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, 10);

    return 0;
}


int test_iteration_count_100(void)
{
    struct timespec start, stop;
    unsigned int total_time = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < 100; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / 100.0;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, 100);

    return 0;
}


int test_iteration_count_1000(void)
{
    struct timespec start, stop;
    unsigned int total_time = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < 1000; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / 1000.0;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, 1000);

    return 0;
}


int test_iteration_count_10000(void)
{
    struct timespec start, stop;
    unsigned int total_time = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < 10000; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / 10000.0;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, 10000);

    return 0;
}


int test_iteration_count_100000(void)
{
    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < 100000; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / 100000.0;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, 100000);

    return 0;
}


int main(void) {
    if (init_test() != 0) {
        printf("init_test");
        return -1;
    }

    test_iteration_count_1();
    test_iteration_count_10();
    test_iteration_count_100();
    test_iteration_count_1000();
    test_iteration_count_10000();
    test_iteration_count_100000();

    return 0;
}