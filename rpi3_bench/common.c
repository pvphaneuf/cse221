#define _GNU_SOURCE

#include <cpufreq.h>
#include <sched.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // ceil()

#include "common.h"


#define CPU_INDEX 3


const unsigned int GET_TIME_OVERHEAD = 469;  // nanoseconds.

const double FOR_LOOP_OVERHEAD = 5.83;  // nanoseconds.

const double SYSCALL_OVERHEAD = 282.5;  // nanoseconds

const double NSEC_PER_CYCLE = 0.833;  // nanoseconds


int init_test() {
    // set the core-affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(CPU_INDEX, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set), &cpu_set);

    // set CPU governor
    unsigned long cpu_max = 0;
    unsigned long cpu_min = 0;
    unsigned long cpu_cur = 0;

    if (cpufreq_cpu_exists(CPU_INDEX) != 0) {
        printf("Invalid CPU index!\n");
        return -1;
    }

    if (cpufreq_get_hardware_limits(CPU_INDEX, &cpu_min, &cpu_max) != 0) {
        printf("Unable to get hardware limits!\n");
        return -1;
    }

    if (cpufreq_set_frequency(CPU_INDEX, cpu_max) != 0) {
        printf("Unable to set frequency(%luMHz) Are u root?\n", cpu_max / 1000);
        return -1;
    }

    cpu_cur = (cpufreq_get_freq_kernel(CPU_INDEX) / 1000);

    printf("Current CPU %u Frequency: %lu MHz\n\n", CPU_INDEX, cpu_cur);

    return 0;
}


int compare_function(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}


void print_results(double result_array[], unsigned int array_size) {
    unsigned int n = 0;
    for (n = 0; n < array_size; n++) {
        printf("%lf ", result_array[n]);
    }
    printf("\n");
}


// Will sort array before finding median.
double get_median(double result_array[], unsigned int array_size) {
    qsort(result_array, array_size, sizeof(int), compare_function);

    // Currently assuming array_size will always be even
    double result_1 = result_array[array_size / 2];
    double result_2 = result_array[(array_size / 2) + 1];

    double median = (result_1 + result_2) / 2;

    return median;
}

double timespec_diff_to_nsecs(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) * 1E9 + \
        (double)(end.tv_nsec - start.tv_nsec);
}

double nsecs_to_cycles(const double time_diff) {
    return ceil(time_diff / NSEC_PER_CYCLE);
}