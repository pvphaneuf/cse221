#include<stdio.h>
#include <stdint.h>/* for uint64 definition */
#include <stdlib.h>/* for exit() definition */
#include <time.h>/* for clock_gettime */

#define BILLION 1000000000L

main()
{
  uint64_t diff;
  struct timespec start, end;
  int i;

  /* measure monotonic time */
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);/* mark start time */
  //sleep(1);/* do stuff */
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);/* mark the end time */

  diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
}
