#include<stdio.h>
#include <stdint.h>/* for uint64 definition */
#include <stdlib.h>/* for exit() definition */
#include <time.h>/* for clock_gettime */

#define BILLION 1000000000L

main()
{
  uint16_t ns;
  
  struct timespec clk_res;

  clock_getres(CLOCK_MONOTONIC_RAW, &clk_res);

  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;

  printf("CLOCK_MONOTONIC_RAW clock resolution: %llu nanoseconds\n", (long long unsigned int) ns);

}
