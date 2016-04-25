#include <sched.h>
#include <stdio.h>
#include <stdlib.h>  // exit()
#include <sys/ipc.h>
#include <sys/shm.h>  // shmget(), shmat()
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>   // timespec, clock_gettime()
#include <unistd.h> // fork()

#include <linux/futex.h>  // futex: fast user-space mutex (fast user-space locking).

#include "common.h"

static inline long long unsigned time_ns(struct timespec *const ts) {
    if (clock_gettime(CLOCK_MONOTONIC_RAW, ts)) {
        exit(1);
    }
    return ((long long unsigned) ts->tv_sec) * 1000000000LLU
           + (long long unsigned) ts->tv_nsec;
}

int main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        exit(EXIT_FAILURE);
    }

    const int iterations = 500000;
    struct timespec ts;

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

    // shmat: attaches the shared memory segment identified by shmid to the address space of the calling process.
    // sharing futex between processes.
    int *futex = shmat(shm_id, NULL, 0);

    // Execution for CHILD process.
    *futex = 0xA;
    if (child_process_pid == 0) {
        for (unsigned int i = 0; i < iterations; i++) {

            // sched_yield() causes the calling thread to relinquish the CPU.  The
            // thread is moved to the end of the queue for its static priority and a
            // new thread gets to run.
            sched_yield();

            // futex will only sleep if *futex = 0xA.
            while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
                // retry
                sched_yield();
            }

            *futex = 0xB;
            while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
                // retry
                sched_yield();
            }
        }
        exit(EXIT_SUCCESS);
    }

    // Execution for PARENT process.
    const long long unsigned start_ns = time_ns(&ts);
    for (unsigned int i = 0; i < iterations; i++) {
        *futex = 0xA;
        while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
            // retry
            sched_yield();
        }
        sched_yield();
        while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
            // retry
            sched_yield();
        }
    }
    const long long unsigned delta = time_ns(&ts) - start_ns;

    const int nswitches = iterations << 2;
    printf("%i process context switches in %lluns (%.1fns/ctxsw)\n",
           nswitches, delta, (delta / (float) nswitches));
    wait(futex);

    exit(EXIT_SUCCESS);
}