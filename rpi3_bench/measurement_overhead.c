#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <time.h>

#include <cpufreq.h>


unsigned int const LOOP_LIMIT = 10000;

unsigned int const NANO_SECONDS_IN_SEC = 1000000000L;

unsigned int const CPU_INDEX = 3;


int init_test()
{	
	// set the core-affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(CPU_INDEX, &cpu_set);
    sched_setaffinity(0, sizeof(cpu_set), &cpu_set);
	
	// set CPU governor
	unsigned long cpu_max = 0;
	unsigned long cpu_min = 0;
	unsigned long cpu_cur = 0;
	
	if (cpufreq_cpu_exists(CPU_INDEX) != 0) {
		printf("Invalid CPU index!\n");
		return -1;
	}

	if (cpufreq_get_hardware_limits(CPU_INDEX, &cpu_min, &cpu_max) != 0) {
		printf("Unable to get hardware limits!\n");
		return -1;
	}

	if (cpufreq_set_frequency(CPU_INDEX, cpu_max) != 0) {
		printf("Unable to set frequency(%luMHz) Are u root?\n", cpu_max / 1000);
		return -1;
	}
	
	cpu_cur = (cpufreq_get_freq_kernel(CPU_INDEX) / 1000);

    printf("Current CPU Frequency: %lu MHz\n", cpu_cur);
	
	return 0;
}


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
			printf("CLOCK_REALTIME clock_gettime");
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
			printf("CLOCK_REALTIME_COARSE clock_gettime");
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
			printf("CLOCK_MONOTONIC clock_gettime");
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
			printf("CLOCK_MONOTONIC_RAW clock_gettime");
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
			printf("CLOCK_MONOTONIC_COARSE clock_gettime");
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
			printf("CLOCK_PROCESS_CPUTIME_ID clock_gettime");
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
			printf("CLOCK_THREAD_CPUTIME_ID clock_gettime");
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
	if(init_test() != 0)
	{
		printf("measure_clocks");
		return -1;
	}

	if(measure_clocks() != 0)
	{
		printf("measure_clocks");
		return -1;
	}
	
	return 0;
}
