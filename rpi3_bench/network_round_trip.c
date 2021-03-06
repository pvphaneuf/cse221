#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()
#include <errno.h>
#include <netinet/in.h>     // sockaddr
#include <arpa/inet.h>      // htons()
#include <sys/socket.h>
#include <time.h>   // timespec, clock_gettime()

#include "common.h"


#define ECHO_SERVICE_PORT 7

#define TCP_TEST_COUNT 50

#define MAX_PAYLOAD_SIZE 1000

#define START_PAYLOAD_SIZE 100

#define PAYLOAD_ITERATION 100


extern int errno;


void
print_errno(void) {
    fprintf(stderr, "Value of errno: %d\n", errno);
    perror("Error printed by perror");
    fprintf(stderr, "Output by strerror: %s\n", strerror(errno));
}


void
warm_up(int sock) {
    unsigned int send_size = START_PAYLOAD_SIZE;
    char send_data_buffer[send_size];
    for (int k = 0; k < send_size; k++) send_data_buffer[k] = 'b';  // stuffs output data buffer full of b's.

    unsigned int receive_size = send_size;
    char receive_data_buffer[receive_size];

    int total_bytes_received = 0;
    int bytes_received = 0;

    send(sock, &send_data_buffer, START_PAYLOAD_SIZE, 0);

    while (total_bytes_received < receive_size) {
        bytes_received = recv(sock, &receive_data_buffer, receive_size, 0);
        total_bytes_received += bytes_received;
    }
}


void
tcp_ping(char ip_address[], int array_size) {
    struct timespec send_start, send_stop;

    int sock;
    unsigned int receive_size;
    int total_bytes_received = 0;

    struct sockaddr_in echo_server_socket;

    memset(&echo_server_socket, 0, sizeof(echo_server_socket)); // Clear struct
    echo_server_socket.sin_family = AF_INET;                    // Internet/IP
    echo_server_socket.sin_addr.s_addr = inet_addr(ip_address);
    echo_server_socket.sin_port = htons(ECHO_SERVICE_PORT);

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to CREATE socket");
        print_errno();
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *) &echo_server_socket, sizeof(echo_server_socket)) < 0) {
        printf("Failed to CONNECT to socket.\n");
        print_errno();
        close(sock);
        exit(EXIT_FAILURE);
    }

    warm_up(sock);

    for (unsigned int send_size = START_PAYLOAD_SIZE; send_size <= MAX_PAYLOAD_SIZE; send_size += PAYLOAD_ITERATION) {

        char send_data_buffer[send_size];
        for (int k = 0; k < send_size; k++) send_data_buffer[k] = 'b';  // stuffs output data buffer full of b's.
        receive_size = send_size;

        int bytes_received = 0;
        char receive_data_buffer[receive_size];

        long long unsigned int total_time = 0;

        for (unsigned int test_idx = 0; test_idx < TCP_TEST_COUNT; test_idx += 1) {

            total_bytes_received = 0;

            clock_gettime(CLOCK_MONOTONIC_RAW, &send_start);

            send(sock, &send_data_buffer, send_size, 0);

            while (total_bytes_received < receive_size) {
                bytes_received = recv(sock, &receive_data_buffer, receive_size, 0);
                total_bytes_received += bytes_received;
            }

            clock_gettime(CLOCK_MONOTONIC_RAW, &send_stop);

            long long unsigned int round_trip_time = 1E9 * (send_stop.tv_sec - send_start.tv_sec)
                                                     + send_stop.tv_nsec - send_start.tv_nsec
                                                     - GET_TIME_OVERHEAD
                                                     - SYSCALL_OVERHEAD * 2;  // for send and receive

//            printf("%llu\n", round_trip_time);

            total_time += round_trip_time;
        }

        printf("%s\tsend_size = %u in %f ms\n", ip_address, send_size, (double)((total_time / 1E6)/ TCP_TEST_COUNT));
    }

    close(sock);
}


int
main(void)
{
    if (init_test() != 0) {
        printf("FAILURE init_test");
        print_errno();
        exit(EXIT_FAILURE);
    }

    char localhost[] = "127.0.0.1";
    tcp_ping(localhost, sizeof(localhost));

    char remote_host[] = "169.254.5.251";
    tcp_ping(remote_host, sizeof(remote_host));
}
