/*
 * This implementation is derivative a of
 * https://github.com/tsuna/contextswitch/blob/master/timectxsw.c
 *
 * License soon to be attached.
 */

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


#define PER_PROCESS_TEST_COUNT 100000

#define PROCESS_COUNT 2

#define PER_PROCESS_PER_ITERATION_CONTEXT_SWITCH_COUNT 2

#define FUTEX_WAKE_OVERHEAD 3818.6  // nsecs


unsigned int get_total_context_switches() {
    return PER_PROCESS_TEST_COUNT * PROCESS_COUNT * PER_PROCESS_PER_ITERATION_CONTEXT_SWITCH_COUNT;
}


int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    struct timespec start, stop;

    // http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmget.html
    // shmget: allocates a System V shared memory segment.
    const int shm_id = shmget(IPC_PRIVATE, // numeric key to be assigned to the returned shared memory segment.
                              sizeof(int), // is the size of the requested shared memory
                              IPC_CREAT | 0666);  // specify the way that the shared memory will be used; creating and granting read and write access to the server
    if (shm_id < 0) {
        printf("FAILURE shmget");
        exit(EXIT_FAILURE);
    }

    // Get child process.
    const pid_t child_pid = fork();
    if (child_pid == -1) {
        printf("FAILURE fork");
        exit(EXIT_FAILURE);
    }

    int *futex = shmat(shm_id, NULL, 0);

    *futex = 0xA;

    // Execution for CHILD process.
    if (child_pid == 0) {
        for (unsigned int i = 0; i < PER_PROCESS_TEST_COUNT; i++) {

            // enable parent process to execute (assuming this is on a single core).
            sched_yield();

            // sleep child process
            while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
                sched_yield();  // retry
            }

            // child process has awoken and will now set futex value to enable parent to sleep.
            *futex = 0xB;

            while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                sched_yield();  // retry
            }
        }
        exit(EXIT_SUCCESS);
    }

    // Execution for PARENT process, and timing it.
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for (unsigned int i = 0; i < PER_PROCESS_TEST_COUNT; i++) {

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
                                              - (FOR_LOOP_OVERHEAD * PER_PROCESS_TEST_COUNT)
                                              - (FUTEX_WAKE_OVERHEAD * PER_PROCESS_TEST_COUNT)
                                              - (SYSCALL_OVERHEAD * 2 * PER_PROCESS_TEST_COUNT); // Two syscalls: FUTEX_WAIT and sched_yield

    printf("%llu\n", total_time);

    const long long unsigned int total_cycles = nsecs_to_cycles(total_time);

    const int context_switch_count = get_total_context_switches();

    printf("%i PROCESS context switches in %llu cycles (%llu cycles per context switch)\n",
           context_switch_count, total_cycles, (total_cycles / context_switch_count));

    int wait_status = 0;
    waitpid(child_pid, &wait_status, 0);

    exit(EXIT_SUCCESS);
}