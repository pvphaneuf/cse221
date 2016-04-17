#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main (void) {
  printf (
	  "%d %d\n",
	  sysconf(_SC_CLK_TCK),  // A.k.a USER_HZ
	          CLOCKS_PER_SEC
	  );
  return 0;
}
