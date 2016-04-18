#include <stdio.h>
#include <time.h>

int main(int argc, char **argv, char **arge) {
  struct timespec tps, tpe;
  if ((clock_gettime(CLOCK_REALTIME, &tps) != 0)
      || (clock_gettime(CLOCK_REALTIME, &tpe) != 0)) {
    perror("clock_gettime");
    return -1;
  }
  printf("%lu s, %lu ns\n", tpe.tv_sec-tps.tv_sec,
	 tpe.tv_nsec-tps.tv_nsec);
  return 0;
}
