#include "common.h"

#define ITERATIONS 100000
#define MEASUREMENTS 300

int proc0() {
    return 0;
}
#define cproc0(i) proc0()

int proc1(int x1) {
    return 0;
}
#define cproc1(i) proc1(i)

int proc2(int x1, int x2) {
    return 0;
}
#define cproc2(i) proc2(i, i)

int proc3(int x1, int x2, int x3) {
    return 0;
}
#define cproc3(i) proc3(i, i, i)

int proc4(int x1, int x2, int x3, int x4) {
    return 0;
}
#define cproc4(i) proc4(i, i, i, i)

int proc5(int x1, int x2, int x3, int x4, int x5) {
    return 0;
}
#define cproc5(i) proc5(i, i, i, i, i)

int proc6(int x1, int x2, int x3, int x4, int x5, int x6) {
    return 0;
}
#define cproc6(i) proc6(i, i, i, i, i, i)

int proc7(int x1, int x2, int x3, int x4, int x5, int x6, int x7) {
    return 0;
}
#define cproc7(i) proc7(i, i, i, i, i, i, i)

double timespec_to_nsecs(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) * 1E9 + \
        (double)(end.tv_nsec - start.tv_nsec);
}

// Not the best code... but it lets us programmatically (ish) select which proc
// to run, without incurring overhead.
#define get_avg_measure(start, end, measures, idx) ({ \
    for (int m = 0; m < MEASUREMENTS; ++m) { \
        clock_gettime(CLOCK_MONOTONIC_RAW, &start); \
        for (int i = 0; i < ITERATIONS; ++i) { \
            cproc ## idx (i); \
        } \
        clock_gettime(CLOCK_MONOTONIC_RAW, &end); \
\
        measures[m] = timespec_to_nsecs(start, end) / ITERATIONS; \
    } \
\
    double avg = 0; \
    for (int m = 0; m < MEASUREMENTS; ++m) { \
        avg += measures[m] / MEASUREMENTS; \
    } \
\
    avg; })

int main() {
    struct timespec start, end;
    double measures[MEASUREMENTS];
    FILE *outfile;

    // set the core-affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(3, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set), &cpu_set);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < ITERATIONS; ++i) {
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double baseline = timespec_to_nsecs(start, end) / ITERATIONS;
    printf("baseline loop: %f\n", baseline);

    printf("0 args: %f\n", get_avg_measure(start, end, measures, 0)-baseline);
    printf("1 args: %f\n", get_avg_measure(start, end, measures, 1)-baseline);
    printf("2 args: %f\n", get_avg_measure(start, end, measures, 2)-baseline);
    printf("3 args: %f\n", get_avg_measure(start, end, measures, 3)-baseline);
    printf("4 args: %f\n", get_avg_measure(start, end, measures, 4)-baseline);
    printf("5 args: %f\n", get_avg_measure(start, end, measures, 5)-baseline);
    printf("6 args: %f\n", get_avg_measure(start, end, measures, 6)-baseline);
    printf("7 args: %f\n", get_avg_measure(start, end, measures, 7)-baseline);
}
