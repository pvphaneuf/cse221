CODEGEN=-mcpu=cortex-a53 -mfpu=neon -mfloat-abi=hard -mapcs-frame
CFLAGS=-O0 -std=gnu11 -Wall $(CODEGEN)
EXES=measurement_overhead hpet_clock_resolutions procedure_call system_call_overhead

all: $(EXES)

.PHONY: clean
clean:
	-rm *.out *.o

measurement_overhead: measurement_overhead.o common.o
	gcc -o measurement_overhead.out measurement_overhead.o common.o -I -lrt -lcpufreq $(CFLAGS)

hpet_clock_resolutions: hpet_clock_resolutions.o
	gcc -o hpet_clock_resolutions.out hpet_clock_resolutions.o $(CFLAGS)

procedure_call: procedure_call.o
	gcc -o procedure_call.out procedure_call.o $(CFLAGS)

system_call_overhead: system_call_overhead.o common.o
	gcc -o system_call_overhead.out system_call_overhead.o common.o -I -lrt -lcpufreq $(CFLAGS)

context_switch: context_switch.o common.o
	gcc -o context_switch.out context_switch.o common.o -lrt -lcpufreq $(CFLAGS)

%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)