#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

    return (timespec_diff_to_nsecs(start, end) / num_threads) - FOR_LOOP_OVERHEAD - GET_TIME_OVERHEAD;
}

double measure_process_creation(int num_processes) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_processes; ++i) {
        if (!fork()) {
            // we're in the child process, so die immediately
            exit(0);
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return (timespec_diff_to_nsecs(start, end) / num_processes) - FOR_LOOP_OVERHEAD - GET_TIME_OVERHEAD;
}

double measure_vprocess_creation(int num_processes) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_processes; ++i) {
        if (!vfork()) {
            // we're in the child process, so die immediately
            exit(0);
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return (timespec_diff_to_nsecs(start, end) / num_processes) - FOR_LOOP_OVERHEAD - GET_TIME_OVERHEAD;
}

double median_30_times(double (*measure)(int), int num_threads) {
    double measure_array[MEASUREMENTS];

    for (int m = 0; m < MEASUREMENTS; ++m) {
        measure_array[m] = measure(NUM_THREADS);
    }

    return get_median(measure_array, MEASUREMENTS);
}

#define print_threads(num_threads)\
    printf("median for %d threads is %f\n", (num_threads), median_30_times(measure_thread_creation, num_threads))

#define print_processes(num_processes)\
    printf("median for %d processes (fork) is %f ms\n", (num_processes), median_30_times(measure_process_creation, num_processes))

#define print_vprocesses(num_processes)\
    printf("median for %d processes (vfork) is %f ms\n", (num_processes), median_30_times(measure_vprocess_creation, num_processes))

int main() {
    assert(init_test() == 0 && "Could not initialize.");

    print_threads(10);
    print_threads(100);
    print_threads(1000);
    print_threads(10000);
    print_threads(100000);

    print_processes(100);
    print_vprocesses(100);

    // No need to join the threads, we don't care how they're killed.
    return 0;
}