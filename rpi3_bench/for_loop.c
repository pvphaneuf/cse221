#include <time.h>
#include <stdio.h>

#include "common.h"


#define TEST_COUNT 1000000


int main(void) {
    if (init_test() != 0) {
        printf("init_test");
        return -1;
    }

    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for(unsigned int count = 0; count < TEST_COUNT; ++count){}
    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    double total_time = timespec_diff_to_nsecs(start, stop);
    total_time = total_time - GET_TIME_OVERHEAD;

    const double average_per_iteration_time = total_time / TEST_COUNT;

    printf("FOR loop latency: %f ns with %u iterations\n", average_per_iteration_time, TEST_COUNT);

    return 0;
}