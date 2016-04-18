all: measurement_overhead hpet_clock_resolutions

measurement_overhead:
	gcc -o measurement_overhead.out measurement_overhead.c -lrt -lcpufreq

hpet_clock_resolutions:
	gcc -o hpet_clock_resolutions.out hpet_clock_resolutions.c

clean:
	-rm *.out
