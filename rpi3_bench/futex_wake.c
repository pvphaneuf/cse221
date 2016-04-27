/*
 * This implementation is derivative a of
 * https://github.com/tsuna/contextswitch/blob/master/timectxsw.c
 *
 * Will have to kill this program with ctrl-C since child process will most likely
 * wait indefinitely.
 */

#define _GNU_SOURCE
#include <cpufreq.h>
#include <sched.h>

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>  // exit()
#include <sys/ipc.h>
#include <sys/shm.h>  // shmget(), shmat()
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>   // timespec, clock_gettime()
#include <unistd.h> // fork()
#include <linux/futex.h>

#include "common.h"


#define TEST_COUNT_PER_PROCESS 100000

#define CHILD_PID 0


int set_process_cpu_and_freq(const unsigned int cpu_index) {
    // set the core-affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_index, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set), &cpu_set);

    // set CPU governor
    unsigned long cpu_max = 0;
    unsigned long cpu_min = 0;
    unsigned long cpu_cur = 0;

    if (cpufreq_cpu_exists(cpu_index) != 0) {
        printf("Invalid CPU index!\n");
        return -1;
    }

    if (cpufreq_get_hardware_limits(cpu_index, &cpu_min, &cpu_max) != 0) {
        printf("Unable to get hardware limits!\n");
        return -1;
    }

    if (cpufreq_set_frequency(cpu_index, cpu_max) != 0) {
        printf("Unable to set frequency(%luMHz) Are u root?\n", cpu_max / 1000);
        return -1;
    }

    cpu_cur = (cpufreq_get_freq_kernel(cpu_index) / 1000);

    printf("Current CPU %u Frequency: %lu MHz\n\n", cpu_index, cpu_cur);

    return 0;
}


int main(void) {

    struct timespec start, stop;

    const int shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        printf("FAILURE shmget");
        exit(EXIT_FAILURE);
    }

    const pid_t child_pid = fork();
    if (child_pid == -1) {
        printf("FAILURE fork");
        exit(EXIT_FAILURE);
    }

    int *futex = shmat(shm_id, NULL, 0);

    *futex = 0xA;

    if (child_pid == CHILD_PID) {

        set_process_cpu_and_freq(2);

        while(1) {
            syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42);
        }
    }


    // Below only executed by parent
    set_process_cpu_and_freq(3);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for (unsigned int i = 0; i < TEST_COUNT_PER_PROCESS; i++) {
        syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42);  // Assume that a successful and unsuccessful FUTEX_WAKE results in the same amount of time.
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    const long long unsigned int total_time = BILLION * (stop.tv_sec - start.tv_sec)
                                              + stop.tv_nsec - start.tv_nsec
                                              - GET_TIME_OVERHEAD
                                              - (TEST_COUNT_PER_PROCESS * FOR_LOOP_OVERHEAD);

    printf("%i FUTEX_WAIT system calls in %lluns (%.1fns per FUTEX_WAKE system calls)\n",
           TEST_COUNT_PER_PROCESS, total_time, (total_time / (float) TEST_COUNT_PER_PROCESS));

    int wait_status = 0;
    waitpid(child_pid, &wait_status, 0);

    exit(EXIT_SUCCESS);
}