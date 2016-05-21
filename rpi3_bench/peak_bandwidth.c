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


#define DISCARD_SERVICE_PORT 9

// TODO: execute more tests.
#define TEST_COUNT = 100


extern int errno;


void
print_errno(void) {
    fprintf(stderr, "Value of errno: %d\n", errno);
    perror("Error printed by perror");
    fprintf(stderr, "Output by strerror: %s\n", strerror(errno));
}


// This peak bandwidth test will assume that the network being used is a direct
// ethernet connection to another machine
void tcp_peak_bandwidth(char ip_address[], int array_size) {
    struct timespec start, stop;

    int sock;

    struct sockaddr_in discard_service_socket;

    memset(&discard_service_socket, 0, sizeof(discard_service_socket)); // Clear struct
    discard_service_socket.sin_family = AF_INET;                    // Internet/IP
    discard_service_socket.sin_addr.s_addr = inet_addr(ip_address);
    discard_service_socket.sin_port = htons(DISCARD_SERVICE_PORT);

    unsigned int send_data_byte_size = 12500000;  // 12500000 bytes = 100 megabits
    char* send_data_buffer;
    send_data_buffer = malloc(send_data_byte_size);
    if(send_data_buffer == NULL) {
        printf("Failed allocate memory to send buffer.\n");
        exit(EXIT_FAILURE);
    }
    memset(send_data_buffer, 'b', send_data_byte_size);

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to CREATE socket.");
        print_errno();
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *) &discard_service_socket, sizeof(discard_service_socket)) < 0) {
        printf("Failed to CONNECT to socket.\n");
        print_errno();
        close(sock);
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    send(sock, &send_data_buffer, send_data_byte_size, 0);

    clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

    close(sock);
    free(send_data_buffer);

    const long long unsigned int total_time = 1E9 * (stop.tv_sec - start.tv_sec)
                                              + stop.tv_nsec - start.tv_nsec;

    printf("%s\t%u bytes in %f ns\n", ip_address, send_data_byte_size, (double)total_time);
}


int
main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        print_errno();
        exit(EXIT_FAILURE);
    }

    char localhost[] = "127.0.0.1";
    tcp_peak_bandwidth(localhost, sizeof(localhost));

    char remote_host[] = "169.254.5.251";
    tcp_peak_bandwidth(remote_host, sizeof(remote_host));
}