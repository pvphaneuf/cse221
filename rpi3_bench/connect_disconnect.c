#include <unistd.h>         // sleep()
#include <stdio.h>
#include <string.h>
#include <unistd.h>         // close()
#include <errno.h>
#include <netinet/in.h>     // sockaddr
#include <arpa/inet.h>      // htons()
#include <sys/socket.h>
#include <time.h>           // timespec, clock_gettime()
#include <stdlib.h>         //EXIT_FAILURE
#include <limits.h>         //ULLONG_MAX

#include "common.h"


#define DISCARD_SERVICE_PORT 9

#define TEST_COUNT 1000

//#define DEBUG


extern int errno;


void
print_errno(void) {
    fprintf(stderr, "Value of errno: %d\n", errno);
    perror("Error printed by perror");
    fprintf(stderr, "Output by strerror: %s\n", strerror(errno));
}


void connect_disconnect(char ip_address[], unsigned int array_size) {
    struct timespec connect_start, connect_stop, close_start, close_stop;

    long long unsigned int connect_time_total = 0;
    long long unsigned int close_time_total = 0;

    int sock;

    struct sockaddr_in discard_service_socket;

    memset(&discard_service_socket, 0, sizeof(discard_service_socket)); // Clear struct
    discard_service_socket.sin_family = AF_INET;                    // Internet/IP
    discard_service_socket.sin_addr.s_addr = inet_addr(ip_address);
    discard_service_socket.sin_port = htons(DISCARD_SERVICE_PORT);

    for (unsigned int count = 0; count < TEST_COUNT; count++) {
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            printf("Failed to CREATE socket.");
            print_errno();
            close(sock);
            exit(EXIT_FAILURE);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &connect_start);

#if defined(DEBUG)
        if (connect(sock, (struct sockaddr *) &discard_service_socket, sizeof(discard_service_socket)) < 0) {
            printf("Failed to CONNECT to socket.\n");
            print_errno();
            close(sock);
            exit(EXIT_FAILURE);
        }
#else
        connect(sock, (struct sockaddr *) &discard_service_socket, sizeof(discard_service_socket));
#endif
        clock_gettime(CLOCK_MONOTONIC_RAW, &connect_stop);
        long long unsigned int connect_time = 1E9 * (connect_stop.tv_sec - connect_start.tv_sec)
                       + connect_stop.tv_nsec - connect_start.tv_nsec
                       - GET_TIME_OVERHEAD - SYSCALL_OVERHEAD;
        connect_time_total += connect_time;

        usleep(1E5);

        clock_gettime(CLOCK_MONOTONIC_RAW, &close_start);
        close(sock);
        clock_gettime(CLOCK_MONOTONIC_RAW, &close_stop);
        long long unsigned int close_time = 1E9 * (close_stop.tv_sec - close_start.tv_sec)
                     + close_stop.tv_nsec - close_start.tv_nsec
                     - GET_TIME_OVERHEAD - SYSCALL_OVERHEAD;
        close_time_total += close_time;

//        printf("connect %f ms\tclose %f ms\n", (double)(connect_time / 1E6), (double)(close_time/ 1E6));
    }

    double connect_time_avg = (double)(connect_time_total / (TEST_COUNT * 1E6));
    double close_time_avg = (double)(close_time_total / (TEST_COUNT * 1E6));

    printf("%s\tconnect avg %f ms\tclose avg %f ms\n", ip_address, connect_time_avg, close_time_avg);
}


int
main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        print_errno();
        exit(EXIT_FAILURE);
    }

    char localhost[] = "127.0.0.1";
    connect_disconnect(localhost, sizeof(localhost));

    char remote_host[] = "169.254.5.251";
    connect_disconnect(remote_host, sizeof(remote_host));
}