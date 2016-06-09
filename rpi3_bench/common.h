#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define BILLION 1E9

extern const unsigned int GET_TIME_OVERHEAD;  // 469 nanoseconds.

extern const double FOR_LOOP_OVERHEAD;  // 5.83 nanoseconds.

extern const double SYSCALL_OVERHEAD;  // 282.5 nanoseconds

extern const double RAND_OVERHEAD;  // 74 nanoseconds

int init_test();

int compare_function(const void *a, const void *b);

double get_median(double result_array[], unsigned int array_size);

double get_stddev(double result_array[], unsigned int array_size);

void print_results(double result_array[], unsigned int array_size);

double timespec_diff_to_nsecs(struct timespec start, struct timespec end);

// Will round up to the nearest cycle count
long long unsigned int nsecs_to_cycles(const long long unsigned int time_diff);

#endif