#include <stdio.h>
#include <string.h>
#include <unistd.h>         // close()
#include <errno.h>
#include <netinet/in.h>     // sockaddr
#include <arpa/inet.h>      // htons()
#include <sys/socket.h>
#include <time.h>   // timespec, clock_gettime()
#include <stdlib.h>  //EXIT_FAILURE
#include <limits.h>  //ULLONG_MAX

#include "common.h"


extern int errno;


void
print_errno(void) {
    fprintf(stderr, "Value of errno: %d\n", errno);
    perror("Error printed by perror");
    fprintf(stderr, "Output by strerror: %s\n", strerror(errno));
}


int
main(void) {
    if (init_test() != 0) {
        printf("FAILURE init_test");
        print_errno();
        exit(EXIT_FAILURE);
    }
}