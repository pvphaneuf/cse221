/*
 * This implementation is derivative a of
 * https://github.com/tsuna/contextswitch/blob/master/timectxsw.c
 *
 * License soon to be attached.
 */


#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>  // exit()
#include <sys/ipc.h>
#include <sys/shm.h>  // shmget(), shmat()
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>   // timespec, clock_gettime()
#include <unistd.h>

#include <linux/futex.h>

#include "common.h"


#define PER_THREAD_TEST_COUNT 100000

#define THREAD_COUNT 2

#define PER_THREAD_PER_ITERATION_CONTEXT_SWITCH_COUNT 2

#define FUTEX_WAKE_OVERHEAD 3818.6  // nsecs


unsigned int get_total_context_switches() {
    return PER_THREAD_TEST_COUNT * THREAD_COUNT * PER_THREAD_PER_ITERATION_CONTEXT_SWITCH_COUNT;
}


static void* thread(void* restrict ftx) {

    int* futex = (int*) ftx;

    for (int i = 0; i < PER_THREAD_TEST_COUNT; i++) {

        sched_yield();

        while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
            sched_yield();  // retry
        }

        *futex = 0xB;

        while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
            sched_yield();  // retry
        }
    }

    return NULL;
}


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    struct timespec start, stop;

    const int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);

    int* futex = shmat(shm_id, NULL, 0);

    pthread_t thd;

    if (pthread_create(&thd, NULL, thread, futex)) {
        exit(EXIT_FAILURE);
    }

    *futex = 0xA;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for (int i = 0; i < PER_THREAD_TEST_COUNT; i++) {

        *futex = 0xA;

        while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
            sched_yield();  // retry
        }

        sched_yield();

        while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
            sched_yield();  // retry
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    const long long unsigned int total_time = BILLION * (stop.tv_sec - start.tv_sec)
                                              + stop.tv_nsec - start.tv_nsec
                                              - GET_TIME_OVERHEAD
                                              - (FOR_LOOP_OVERHEAD * PER_THREAD_TEST_COUNT)
                                              - (FUTEX_WAKE_OVERHEAD * PER_THREAD_TEST_COUNT)
                                              - (SYSCALL_OVERHEAD * 2 * PER_THREAD_TEST_COUNT); // Two syscalls: FUTEX_WAIT and sched_yield

    const long long unsigned int total_cycles = nsecs_to_cycles(total_time);

    const int context_switch_count = get_total_context_switches();

    printf("%i THREAD context switches in %llu cycles (%llu cycles per context switch)\n",
           context_switch_count, total_cycles, (total_cycles / context_switch_count));

    int wait_status = 0;
    wait(&wait_status);

    return 0;
}