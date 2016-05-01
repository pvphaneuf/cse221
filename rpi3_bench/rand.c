#include <stdio.h>
#include <stdlib.h>  // srand(), rand()
#include <time.h>  // time

#include "common.h"

#define TEST_COUNT 100000


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    struct timespec start, end;

    srand(time(NULL));

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int j = 0; j < TEST_COUNT; j++) {
        int num = rand();
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    const long long unsigned int total_time = BILLION * (end.tv_sec - start.tv_sec)
                                              + end.tv_nsec - start.tv_nsec
                                              - GET_TIME_OVERHEAD
                                              - (FOR_LOOP_OVERHEAD * TEST_COUNT);

    double average = (total_time / TEST_COUNT);
    printf("Overhead for calling rand(): %f\n", average);

}