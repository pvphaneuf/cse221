#include <time.h>
#include <stdio.h>

#include "common.h"


const unsigned int CLOCK_TEST_COUNT = 10000;

const unsigned int FOR_LOOP_TEST_COUNT = 100;


int measure_clocks(void) {
    struct timespec start, stop;

    unsigned int diff = 0;
    unsigned int result_array_idx = 0;

    double result_array[CLOCK_TEST_COUNT];

    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_REALTIME, &start) != 0) || (clock_gettime(CLOCK_REALTIME, &stop) != 0)) {
            printf("CLOCK_REALTIME clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_REALTIME: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));


    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_REALTIME_COARSE, &start) != 0) || (clock_gettime(CLOCK_REALTIME_COARSE, &stop) != 0)) {
            printf("CLOCK_REALTIME_COARSE clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_REALTIME_COARSE: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));


    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_MONOTONIC, &start) != 0) || (clock_gettime(CLOCK_MONOTONIC, &stop) != 0)) {
            printf("CLOCK_MONOTONIC clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_MONOTONIC: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));


    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_MONOTONIC_RAW, &start) != 0) || (clock_gettime(CLOCK_MONOTONIC_RAW, &stop) != 0)) {
            printf("CLOCK_MONOTONIC_RAW clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_MONOTONIC_RAW: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));


    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_MONOTONIC_COARSE, &start) != 0) ||
            (clock_gettime(CLOCK_MONOTONIC_COARSE, &stop) != 0)) {
            printf("CLOCK_MONOTONIC_COARSE clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_MONOTONIC_COARSE: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));


    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) != 0) ||
            (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop) != 0)) {
            printf("CLOCK_PROCESS_CPUTIME_ID clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_PROCESS_CPUTIME_ID: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));


    for (result_array_idx = 0; result_array_idx < CLOCK_TEST_COUNT; result_array_idx++) {
        if ((clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start) != 0) ||
            (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop) != 0)) {
            printf("CLOCK_THREAD_CPUTIME_ID clock_gettime");
            return -1;
        }

        diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        result_array[result_array_idx] = diff;
    }

    printf("CLOCK_THREAD_CPUTIME_ID: %f ns\n", get_median(result_array, CLOCK_TEST_COUNT));

    return 0;
}


int measure_for_loop(unsigned int test_iteration_count)
{
	struct timespec start, stop;
	unsigned int total_time = 0;

    double result_array[FOR_LOOP_TEST_COUNT];

    for(unsigned int result_array_idx = 0; result_array_idx < FOR_LOOP_TEST_COUNT; result_array_idx++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for(unsigned int count = 0; count < test_iteration_count; count++){}
        clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

        total_time = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;

        //  To deal with if total time is less than GET_TIME_OVERHEAD
        if(total_time < GET_TIME_OVERHEAD) {
            total_time = 0;
        }
        else {
            total_time = total_time - GET_TIME_OVERHEAD;
        }

        double avg_iteration_time = (double)total_time / (double)test_iteration_count;

        result_array[result_array_idx] = avg_iteration_time;
    }

    printf("FOR loop latency: %f ns with %u iterations\n", get_median(result_array, FOR_LOOP_TEST_COUNT), test_iteration_count);

	return 0;
}


int main(void) {
    if (init_test() != 0) {
        printf("init_test");
        return -1;
    }

    if (measure_clocks() != 0) {
        printf("measure_clocks");
        return -1;
    }

	measure_for_loop(1);
	measure_for_loop(10);
	measure_for_loop(100);
	measure_for_loop(1000);
	measure_for_loop(10000);
    measure_for_loop(100000);

    return 0;
}
