CODEGEN=-mcpu=cortex-a53 -mfpu=neon -mfloat-abi=hard -mapcs-frame
CFLAGS=-O0 -std=gnu11 -Wall $(CODEGEN)
EXES=measurement_overhead hpet_clock_resolutions procedure_call system_call_overhead task_creation for_loop
COMMON_FLAGS=common.o -lrt -lcpufreq -pthread $(CFLAGS)

all: $(EXES)

.PHONY: clean
clean:
	-rm *.out *.o

measurement_overhead: measurement_overhead.o common.o
	gcc -o measurement_overhead.out measurement_overhead.o $(COMMON_FLAGS)

for_loop: for_loop.o common.o
	gcc -o for_loop.out for_loop.o $(COMMON_FLAGS)

hpet_clock_resolutions: hpet_clock_resolutions.o
	gcc -o hpet_clock_resolutions.out hpet_clock_resolutions.o $(CFLAGS)

procedure_call: procedure_call.o common.o
	gcc -o procedure_call.out procedure_call.o $(COMMON_FLAGS)

system_call_overhead: system_call_overhead.o common.o
	gcc -o system_call_overhead.out system_call_overhead.o $(COMMON_FLAGS)

task_creation: task_creation.o common.o
	gcc -o task_creation.out task_creation.o $(COMMON_FLAGS)

%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)
