#include "common.h"

main()
{
  uint64_t ns;
  
  struct timespec clk_res;

  clock_getres(CLOCK_REALTIME, &clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_REALTIME clock resolution: %" PRIu64 " nanoseconds\n", ns);
  
  clock_getres(CLOCK_REALTIME_COARSE, &clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_REALTIME_COARSE clock resolution: %" PRIu64 " nanoseconds\n", ns);
  
  clock_getres(CLOCK_MONOTONIC, &clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_MONOTONIC clock resolution: %" PRIu64 " nanoseconds\n", ns);
  
  clock_getres(CLOCK_MONOTONIC_RAW, & clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_MONOTONIC_RAW clock resolution: %" PRIu64 " nanoseconds\n", ns);
  
  clock_getres(CLOCK_MONOTONIC_COARSE, &clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_MONOTONIC_COARSE clock resolution: %" PRIu64 " nanoseconds\n", ns);
  
  clock_getres(CLOCK_PROCESS_CPUTIME_ID, &clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_PROCESS_CPUTIME_ID clock resolution: %" PRIu64 " nanoseconds\n", ns);
  
  clock_getres(CLOCK_THREAD_CPUTIME_ID, &clk_res);
  ns = BILLION * clk_res.tv_sec + clk_res.tv_nsec;
  printf("CLOCK_THREAD_CPUTIME_ID clock resolution: %" PRIu64 " nanoseconds\n", ns);
}
