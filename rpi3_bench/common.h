#ifndef COMMON_H
#define COMMON_H

#define BILLION 1E9

extern const unsigned int GET_TIME_OVERHEAD;  // 469 nanoseconds.

extern const float FOR_LOOP_OVERHEAD;  // 6.7 nanoseconds.

extern const unsigned int CPU_INDEX;  // CPU 3

int init_test();

int compare_function(const void *a, const void *b);

double get_median(double result_array[], unsigned int array_size);

void print_results(double result_array[], unsigned int array_size);

double timespec_diff_to_nsecs(struct timespec start, struct timespec end);

#endif