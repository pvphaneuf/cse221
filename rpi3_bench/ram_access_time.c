//#include <unistd.h>  // getpagesize()
#include <stdio.h>
#include <math.h>  //pow(), log2()
#include <stdlib.h>  // srand(), rand(), malloc()
#include <string.h>  // memset()
#include <time.h>  // time
#include <limits.h>

#include "common.h"

#define TEST_COUNT 100000

#define MAX_ARRAY_SIZE_POWER 30

#define START_ARRAY_SIZE_POWER 2

#define SIZE_LEN MAX_ARRAY_SIZE_POWER - START_ARRAY_SIZE_POWER


void test_4Bytes_to_500MBytes(void) {
    struct timespec start, end;

    // TODO: use different size iterations.
    int sizes[SIZE_LEN];

    for (int power_index = START_ARRAY_SIZE_POWER; power_index < MAX_ARRAY_SIZE_POWER; power_index++) {
        sizes[power_index - START_ARRAY_SIZE_POWER] = pow(2, power_index);
    }

    srand(time(NULL));

    for (int memory_size_iterator = 0; memory_size_iterator < SIZE_LEN; memory_size_iterator++) {

        int size = (int) sizes[memory_size_iterator];

        int *memory_pointer = (int *) malloc(size);  // allocate memory for test.
        if (!memory_pointer) {
            printf("FAILURE Couldn't allocate memory of %i bytes\n", size);
            exit(EXIT_FAILURE);
        }

        // Initialize memory region.
        memset(memory_pointer, INT_MAX, size);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for (int test_iteration = 0; test_iteration < TEST_COUNT; test_iteration++) {

            // Using random stride size, since assuming we don't know cache line size.
            int stride = rand() % (size / sizeof(int));

            // Integer access, as directed by project description.
            int touch = *(memory_pointer + stride);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        free(memory_pointer);

        const long long unsigned int total_time = timespec_diff_to_nsecs(start, end)
                                                  - GET_TIME_OVERHEAD
                                                  - (FOR_LOOP_OVERHEAD * TEST_COUNT)
                                                  - (FOR_LOOP_OVERHEAD * RAND_OVERHEAD);

        const long long unsigned int total_cycles = nsecs_to_cycles(total_time);

        double average_time = (total_time / TEST_COUNT);

        printf("%i\t%i\t%f\t%llu\n", size, (int) log2(size), average_time, (total_cycles / TEST_COUNT));
    }
}


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    test_4Bytes_to_500MBytes();

    exit(EXIT_SUCCESS);
}