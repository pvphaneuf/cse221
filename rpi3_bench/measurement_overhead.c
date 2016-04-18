#include <stdio.h>
#include <time.h>


unsigned int const LOOP_LIMIT = 10000;

unsigned int const NANO_SECONDS_IN_SEC = 1000000000L;


int measure_clocks(void)
{
	struct timespec start, stop;
	
	unsigned int diff = 0;
	unsigned int sum = 0;
	float avg = 0;	
	
	unsigned int count;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_REALTIME, &start) != 0) || (clock_gettime(CLOCK_REALTIME, &stop) != 0))
		{
			perror("CLOCK_REALTIME clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_REALTIME: %f ns\n", avg);
	
	
	diff = 0;
	sum = 0;
	avg = 0;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_REALTIME_COARSE, &start) != 0) || (clock_gettime(CLOCK_REALTIME_COARSE, &stop) != 0))
		{
			perror("CLOCK_REALTIME_COARSE clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_REALTIME_COARSE: %f ns\n", avg);
	
	
	diff = 0;
	sum = 0;
	avg = 0;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_MONOTONIC, &start) != 0) || (clock_gettime(CLOCK_MONOTONIC, &stop) != 0))
		{
			perror("CLOCK_MONOTONIC clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_MONOTONIC: %f ns\n", avg);
	
	
	diff = 0;
	sum = 0;
	avg = 0;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_MONOTONIC_RAW , &start) != 0) || (clock_gettime(CLOCK_MONOTONIC_RAW, &stop) != 0))
		{
			perror("CLOCK_MONOTONIC_RAW clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_MONOTONIC_RAW: %f ns\n", avg);
	
	
	diff = 0;
	sum = 0;
	avg = 0;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_MONOTONIC_COARSE  , &start) != 0) || (clock_gettime(CLOCK_MONOTONIC_COARSE, &stop) != 0))
		{
			perror("CLOCK_MONOTONIC_COARSE clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_MONOTONIC_COARSE: %f ns\n", avg);
	
	
	diff = 0;
	sum = 0;
	avg = 0;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_PROCESS_CPUTIME_ID  , &start) != 0) || (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop) != 0))
		{
			perror("CLOCK_PROCESS_CPUTIME_ID clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_PROCESS_CPUTIME_ID: %f ns\n", avg);
	
	
	diff = 0;
	sum = 0;
	avg = 0;
	for(count = 0; count < LOOP_LIMIT; count++)
	{
		if ((clock_gettime(CLOCK_THREAD_CPUTIME_ID  , &start) != 0) || (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop) != 0))
		{
			perror("CLOCK_THREAD_CPUTIME_ID clock_gettime");
			return -1;
		}
		
		diff = NANO_SECONDS_IN_SEC * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		sum = sum + diff;
	}
	avg = (float)sum / (float)LOOP_LIMIT;
	printf("CLOCK_THREAD_CPUTIME_ID: %f ns\n", avg);
		
	return 0;
}


int main(void)
{	
	if(measure_clocks() != 0)
	{
		perror("measure_clocks");
		return -1;
	}
	
	return 0;
}
