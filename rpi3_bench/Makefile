CODEGEN=-mcpu=cortex-a53 -mfpu=neon -mfloat-abi=hard -mapcs-frame
CFLAGS=-O0 -std=gnu11 $(CODEGEN)
EXES=measurement_overhead hpet_clock_resolutions

all: $(EXES)

.PHONY: clean
clean:
	-rm *.out

measurement_overhead: measurement_overhead.o
	gcc -o measurement_overhead.out measurement_overhead.o -lrt -lcpufreq $(CFLAGS)

hpet_clock_resolutions: hpet_clock_resolutions.o
	gcc -o hpet_clock_resolutions.out hpet_clock_resolutions.o $(CFLAGS)

%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)