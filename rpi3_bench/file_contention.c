#define _GNU_SOURCE

#include "common.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int id;
    pthread_barrier_t *barrier;
} baton;

// Queried system for this number
#define BLOCK_SIZE 4096

// Number of other simultaneous users
#define NUM_THREADS 10

// Size of each of the files created
#define NUM_BLOCKS (1 << 13)

// WARNING: These functions specifically are optimized! 
//double measure_fc_access(int, int, bool) __attribute__((optimize("Ofast")));


double measure_fc_access(int fd, int num_blocks) {
    struct timespec start, end;
    char* buf = NULL;

    // Flush caches. We shouldn't be using them, but this enforces it.
    assert(system("sync; echo 1 >|/proc/sys/vm/drop_caches") == 0);

    // Align for O_DIRECT.
    posix_memalign((void**)&buf, BLOCK_SIZE, BLOCK_SIZE);
    if (buf == NULL) {
        puts("Error allocating aligned mem.\n");
        exit(1);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < num_blocks; ++i) {
        pread(fd, buf, BLOCK_SIZE, i*BLOCK_SIZE);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // Delete aligned mem.
    free(buf);

    return timespec_diff_to_nsecs(start, end) - num_blocks*FOR_LOOP_OVERHEAD - GET_TIME_OVERHEAD;
}

void measure_read_time(int fd, int num_threads) {
    // Measure the time loading it.
    double read_time = measure_fc_access(fd, NUM_BLOCKS);

    printf("%d threads, %d MB: %.2f us/block\n",
           num_threads,
           NUM_BLOCKS * BLOCK_SIZE / 1024 / 1024,
           read_time / 1000 / NUM_BLOCKS);
}

void write_to_disk(const char *filename, int num_blocks) {
    char buf[1024];

    // Use the system PRNG.
    snprintf(buf, 1024, "dd if=/dev/urandom of=%s count=%d bs=4096 status=none", filename, num_blocks);
    assert(system(buf) == 0);
}

void *child(void *aux) {
    char filename[64];
    baton *b = (baton *)aux;
    char *buf;

    snprintf(filename, 64, "test%d.file", b->id);
    write_to_disk(filename, NUM_BLOCKS);

    // Align for O_DIRECT.
    posix_memalign((void**)&buf, BLOCK_SIZE, BLOCK_SIZE);
    if (buf == NULL) {
        puts("Error allocating aligned mem.\n");
        exit(1);
    }

    // Open the test file.
    int fd = open(filename, O_SYNC | O_DIRECT | O_RDONLY);
    assert(fd != -1 && "open() failed.");

    // Tell main thread we're ready.
    pthread_barrier_wait(b->barrier);

    // Read forever.
    while (1) {
        pread(fd, buf, BLOCK_SIZE, (rand() % NUM_BLOCKS) * BLOCK_SIZE);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    assert(init_test() == 0 && "Could not initialize.");

    // Make our own test file
    puts("Writing test file...");
    write_to_disk("test.file", NUM_BLOCKS);

    // Open the test file.
    int fd = open("test.file", O_SYNC | O_DIRECT | O_RDONLY);
    assert(fd != -1 && "open() failed.");

    for (int i = 0; i < NUM_THREADS; ++i) {
        // Make baton
        pthread_barrier_t barrier;
        baton b;

        // Initialize barrier
        assert(pthread_barrier_init(&barrier, NULL, 2) == 0);

        b.id = i;
        b.barrier = &barrier;

        printf("Starting thread %d\n", i);
        int err = pthread_create(&threads[i], NULL, child, &b);

        if (err != 0) {
            printf("error: %s", strerror(err));
            exit(1);
        }

        // wait for thread to set up
        pthread_barrier_wait(&barrier);
        usleep(500);

        // Measure time to read
        measure_read_time(fd, i+1);
    }

    // Close the file.
    close(fd);

    // Delete the file.
    unlink("test.file");

    return 0;
}
