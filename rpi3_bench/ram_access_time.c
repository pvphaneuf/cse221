//#include <unistd.h>  // getpagesize()
#include <stdio.h>
#include <math.h>
#include <stdlib.h>  // srand(), rand()
#include <time.h>  // time

#include "common.h"


// L1 Cache = 32 KB
// L2 Cache = 512 KB


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

        // Set all int size locations in memory to 1.
        for (int memory_index = 0; memory_index < size / sizeof(int); memory_index++) {
            memory_pointer[memory_index] = 1;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for (int test_iteration = 0; test_iteration < TEST_COUNT; test_iteration++) {

            // Using random stride size, since assuming we don't know cache line size.
            int stride = rand() % (size / sizeof(int));

            // Integer access, as directed by project description.
            int val = *(memory_pointer + stride);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        free(memory_pointer);

        const long long unsigned int total_time = timespec_diff_to_nsecs(start, end)
                                                  - GET_TIME_OVERHEAD
                                                  - (FOR_LOOP_OVERHEAD * TEST_COUNT)
                                                  - (FOR_LOOP_OVERHEAD * RAND_OVERHEAD);

        double average = (total_time / TEST_COUNT);
        printf("Memory Bytes Size: %i\tAverage Access Latency: %f\n", size, average);
    }
}

void test_500MBytes_to_800MBytes(void) {
    struct timespec start, end;

    int sizes[3];

    // Incrementing by 50 MBs
    for (int index = 0; index < 6; index++) {
        sizes[index] = pow(2, 29)
                       + (524288 * 100) * (index + 1); // going up by 50 MBs.
    }

    srand(time(NULL));

    for (int memory_size_iterator = 0; memory_size_iterator < 3; memory_size_iterator++) {

        int size = (int) sizes[memory_size_iterator];
        int *memory_pointer = (int *) malloc(size);  // allocate memory for test.
        if (!memory_pointer) {
            printf("FAILURE Couldn't allocate memory of %i bytes\n", size);
            exit(EXIT_FAILURE);
        }

        // Set all int size locations in memory to 1.
        for (int memory_index = 0; memory_index < size / sizeof(int); memory_index++) {
            memory_pointer[memory_index] = 1;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for (int test_iteration = 0; test_iteration < TEST_COUNT; test_iteration++) {

            // Using random stride size, since assuming we don't know cache line size.
            int stride = rand() % (size / sizeof(int));

            // Integer access, as directed by project description.
            int val = *(memory_pointer + stride);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        free(memory_pointer);

        const long long unsigned int total_time = BILLION * (end.tv_sec - start.tv_sec)
                                                  + end.tv_nsec - start.tv_nsec
                                                  - GET_TIME_OVERHEAD
                                                  - (FOR_LOOP_OVERHEAD * TEST_COUNT)
                                                  - (FOR_LOOP_OVERHEAD * RAND_OVERHEAD);

        double average = (total_time / TEST_COUNT);
        printf("Memory Bytes Size: %i\tAverage Access Latency: %f\n", size, average);
    }
}


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    test_4Bytes_to_500MBytes();

//    test_500MBytes_to_800MBytes();

    exit(EXIT_SUCCESS);
}