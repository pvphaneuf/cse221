#include "common.h"

#include <cpufreq.h>


// If I change this, change get_median
unsigned int const TEST_COUNT = 100;


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


int compare_function(const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}


void print_results(float result_array[])
{
	unsigned int n = 0;
	for( n = 0 ; n < TEST_COUNT; n++ ) 
	{
		printf("%f ", result_array[n]);
	}
	printf("\n");
}


// Will sort array before finding median.
float get_median(float result_array[], int array_size)
{
	qsort(result_array, TEST_COUNT, sizeof(int), compare_function);
	
	// Currently assuming array_size will always be even
	float result_1 = result_array[array_size / 2];
	float result_2 = result_array[(array_size / 2) + 1];
	
	float median = (result_1 + result_2) / 2;
	
	return median;
}

int measure_clocks(void)
{	
	struct timespec start, stop;
	
	unsigned int diff = 0;
	unsigned int result_array_idx = 0;
	
	float result_array[TEST_COUNT];

	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_REALTIME, &start) != 0) || (clock_gettime(CLOCK_REALTIME, &stop) != 0))
		{
			printf("CLOCK_REALTIME clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_REALTIME: %f ns\n", get_median(result_array, TEST_COUNT));
	
	
	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_REALTIME_COARSE, &start) != 0) || (clock_gettime(CLOCK_REALTIME_COARSE, &stop) != 0))
		{
			printf("CLOCK_REALTIME_COARSE clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_REALTIME_COARSE: %f ns\n", get_median(result_array, TEST_COUNT));
	
	
	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_MONOTONIC, &start) != 0) || (clock_gettime(CLOCK_MONOTONIC, &stop) != 0))
		{
			printf("CLOCK_MONOTONIC clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_MONOTONIC: %f ns\n", get_median(result_array, TEST_COUNT));
	
	
	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_MONOTONIC_RAW, &start) != 0) || (clock_gettime(CLOCK_MONOTONIC_RAW, &stop) != 0))
		{
			printf("CLOCK_MONOTONIC_RAW clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_MONOTONIC_RAW: %f ns\n", get_median(result_array, TEST_COUNT));
	
	
	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_MONOTONIC_COARSE, &start) != 0) || (clock_gettime(CLOCK_MONOTONIC_COARSE, &stop) != 0))
		{
			printf("CLOCK_MONOTONIC_COARSE clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_MONOTONIC_COARSE: %f ns\n", get_median(result_array, TEST_COUNT));
	
	
	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start) != 0) || (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop) != 0))
		{
			printf("CLOCK_PROCESS_CPUTIME_ID clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_PROCESS_CPUTIME_ID: %f ns\n", get_median(result_array, TEST_COUNT));
	
	
	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		if ((clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start) != 0) || (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop) != 0))
		{
			printf("CLOCK_THREAD_CPUTIME_ID clock_gettime");
			return -1;
		}
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
		
	printf("CLOCK_THREAD_CPUTIME_ID: %f ns\n", get_median(result_array, TEST_COUNT));
	
	return 0;
}


int measure_for_loop(void)
{
	struct timespec start, stop;
	
	unsigned int diff = 0;
	unsigned int result_array_idx = 0;
	
	float result_array[TEST_COUNT];

	for(result_array_idx = 0; result_array_idx < TEST_COUNT; result_array_idx++)
	{
		unsigned int count = 0;
		
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		for(count = 0; count <= 9; count++){;}
		clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
		
		diff = BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec;
		
		result_array[result_array_idx] = diff;
	}
	
	//print_results(result_array);
		
	printf("for loop latency: %f ns\n", get_median(result_array, TEST_COUNT));
	
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
	
	measure_for_loop();
	
	return 0;
}
