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


const unsigned int TEST_COUNT = 100000;


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
        for (unsigned int i = 0; i < TEST_COUNT; i++) {

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

    for (unsigned int i = 0; i < TEST_COUNT; i++) {

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
                                              - (TEST_COUNT * FOR_LOOP_OVERHEAD);

    const int context_switch_count = TEST_COUNT << 2;
    printf("%i process context switches in %lluns (%.1fns per context switch)\n",
           context_switch_count, total_time, (total_time / (float) context_switch_count));

    int wait_status = 0;
    waitpid(child_pid, &wait_status, 0);

    exit(EXIT_SUCCESS);
}