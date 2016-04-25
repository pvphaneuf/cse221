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


const unsigned int TEST_COUNT = 500000;


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
    const pid_t child_process_pid = fork();
    if (child_process_pid == -1) {
        printf("FAILURE fork");
        exit(EXIT_FAILURE);
    }

    int *futex = shmat(shm_id, NULL, 0);

    *futex = 0xA;

    // Execution for CHILD process.
    if (child_process_pid == 0) {
        for (unsigned int i = 0; i < TEST_COUNT; i++) {

            sched_yield();

            // sleep child process
            while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
                // retry
                sched_yield();
            }

            // child process has awoken and will now set futex value to enable parent to sleep.
            *futex = 0xB;
            while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                // retry
                sched_yield();
            }
        }
        exit(EXIT_SUCCESS);
    }

    // Execution for PARENT process, and timing it.
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for (unsigned int i = 0; i < TEST_COUNT; i++) {

        // Wake child process
        *futex = 0xA;
        while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
            // retry
            sched_yield();
        }
        sched_yield();

        // Sleep parent process.
        while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
            // retry
            sched_yield();
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    const long long unsigned int total_time = BILLION * (stop.tv_sec - start.tv_sec)
                                              + stop.tv_nsec - start.tv_nsec
                                              - GET_TIME_OVERHEAD
                                              - (TEST_COUNT * FOR_LOOP_OVERHEAD);

    const int nswitches = TEST_COUNT << 2;
    printf("%i process context switches in %lluns (%.1fns per context switch)\n",
           nswitches, total_time, (total_time / (float) nswitches));

    wait(futex);

    exit(EXIT_SUCCESS);
}