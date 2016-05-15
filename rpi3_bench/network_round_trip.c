#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // close()

#include <errno.h>

#include <netinet/in.h>     // struct sockaddr
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common.h"


extern int errno ;


void
print_errno(void) {
    fprintf(stderr, "Value of errno: %d\n", errno);
    perror("Error printed by perror");
    fprintf(stderr, "Output by strerror: %s\n", strerror(errno));
}


int
main(void)
{
    if (init_test() != 0) {
        printf("FAILURE init_test");
        print_errno();
        exit(EXIT_FAILURE);
    }

    int sock;
    unsigned int receive_size;
    int received = 0;

    struct sockaddr_in echoserver;

    memset(&echoserver, 0, sizeof(echoserver));     /* Clear struct */
    echoserver.sin_family = AF_INET;                /* Internet/IP */
    char local[] = "127.0.0.1";
    echoserver.sin_addr.s_addr = inet_addr(local);  /* IP address */
    echoserver.sin_port = htons(atoi("1000"));         /* server echo port */

    int send_size = 100;

    char send_data_buffer[send_size];
    for (int k = 0; k < send_size; k++) send_data_buffer[k] = 'b';  // stuffs output data buffer full of b's.
    receive_size = send_size;

    printf("%s\n", send_data_buffer);

    int bytes_received = 0;
    char receive_data_buffer[receive_size];
    memset(&receive_data_buffer, 0, sizeof(receive_data_buffer));   // Clearing buffer.

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to CREATE socket");
        print_errno();
        exit(EXIT_FAILURE);
    }

    received = 0;

    if (connect(sock, (struct sockaddr *) &echoserver, sizeof(echoserver)) != 0) {
        printf("Failed to CONNECT to socket.\n");
        print_errno();
        exit(EXIT_FAILURE);
    }

    int bytes_sent = send(sock, &send_data_buffer, send_size, 0);

    printf("%i\n", bytes_sent);

    while (received < receive_size) {
        bytes_received = recv(sock, &receive_data_buffer, receive_size, 0);
        received += bytes_received;
    }

    close(sock);
}