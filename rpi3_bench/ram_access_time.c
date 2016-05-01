//#include <unistd.h>  // getpagesize()
#include <stdio.h>
#include <math.h>
#include <stdlib.h>  // srand(), rand()
#include <time.h>  // time

#include "common.h"


// page size = 4096 Bytes = 1024 ints
// L1 Cache = 32 KB = 8192 ints = 8 pages
// L2 Cache = 512 KB = 131,072 ints = 32 pages


#define TEST_COUNT 100000

#define MAX_ARRAY_SIZE_POWER 30

#define START_ARRAY_SIZE_POWER 2

#define SIZE_LEN MAX_ARRAY_SIZE_POWER - START_ARRAY_SIZE_POWER


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    struct timespec start, end;

    // TODO: use different size iterations.
    int sizes[SIZE_LEN];

    for (int power_index = START_ARRAY_SIZE_POWER; power_index < MAX_ARRAY_SIZE_POWER; power_index++) {
        sizes[power_index - START_ARRAY_SIZE_POWER] = pow(2, power_index);
    }

    srand(time(NULL));

    for (int k = 0; k < SIZE_LEN; k++) {

        int size = (int) sizes[k];  // TODO: after establishing results, try using an array rather than a pointer to an array and see what the difference is.
        int *memory_pointer = (int *) malloc(size);  // allocate memory for test.
        if (!memory_pointer) {
            printf("FAILURE Couldn't allocate memory of %i bytes\n", size);
            exit(EXIT_FAILURE);
        }

        // Set all int size locations in memory to 1.
        for (int t = 0; t < size / sizeof(int); t++) {
            memory_pointer[t] = 1;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        for (int j = 0; j < TEST_COUNT; j++) {

            // Using random stride size, since assuming we don't know cache line size.
            int stride = rand() % (size / sizeof(int));  // TODO: this is going to be considered in timing. Will have to handle.
            int s = *(memory_pointer + stride);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        free(memory_pointer);

        const long long unsigned int total_time = BILLION * (end.tv_sec - start.tv_sec)
                                                  + end.tv_nsec - start.tv_nsec
                                                  - GET_TIME_OVERHEAD
                                                  - (FOR_LOOP_OVERHEAD * TEST_COUNT);

        double average = (total_time / TEST_COUNT);
        printf("Int: %i\t%f\n", size, average);
    }

    exit(EXIT_SUCCESS);
}