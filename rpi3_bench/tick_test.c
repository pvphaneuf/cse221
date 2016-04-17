#define BCM2708_ST_BASE 0x20003000 /* BCM 2835 System Timer */

volatile unsigned *TIMER_registers;

unsigned int TIMER_GetSysTick()
{
  return TIMER_registers[1];
}

void TIMER_Init()
{
  /* open /dev/mem */
  int TIMER_memFd;
  if ((TIMER_memFd = open("/dev/mem", O_RDWR/*|O_SYNC*/) ) < 0)
    {
      printf("can't open /dev/mem - need root ?\n");
      exit(-1);
    }

  /* mmap BCM System Timer */
  void *TIMER_map = mmap(
			 NULL,
			 4096, /* BLOCK_SIZE */
			 PROT_READ /*|PROT_WRITE*/,
			 MAP_SHARED,
			 TIMER_memFd,
			         BCM2708_ST_BASE
			 );

  close(TIMER_memFd);

  if (TIMER_map == MAP_FAILED)
    {
      printf("mmap error %d\n", (int)TIMER_map);
      exit(-1);
    }
  TIMER_registers = (volatile unsigned *)TIMER_map;
}
