#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "common.h"

#define NUM_THREADS 100
#define MEASUREMENTS 30

void *empty_thread(void *a) {
    pthread_exit(NULL);
}

double measure_thread_creation(int num_threads) {
    pthread_t threads[num_threads];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], NULL, empty_thread, NULL);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return (timespec_diff_to_nsecs(start, end) / num_threads) - GET_TIME_OVERHEAD;
}

double median_30_times(int num_threads) {
    double measure_array[MEASUREMENTS];

    for (int m = 0; m < MEASUREMENTS; ++m) {
        measure_array[m] = measure_thread_creation(NUM_THREADS);
    }

    return get_median(measure_array, MEASUREMENTS);
}

#define print_threads(num_threads)\
    printf("median for %d threads is %f\n", (num_threads), median_30_times(num_threads)-FOR_LOOP_OVERHEAD)

int main() {
    assert(init_test() == 0 && "Could not initialize.");

    print_threads(10);
    print_threads(100);
    print_threads(1000);
    print_threads(10000);
    print_threads(100000);

    // No need to join the threads, we don't care how they're killed.
    return 0;
}