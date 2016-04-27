CODEGEN=-mcpu=cortex-a53 -mfpu=neon -mfloat-abi=hard -mapcs-frame
CFLAGS=-O0 -std=gnu11 -Wall $(CODEGEN)
EXES=clock_gettime hpet_clock_resolutions procedure_call system_call_overhead task_creation for_loop futex_wake futex_wait
COMMON_FLAGS=common.o -lm -lrt -lcpufreq -pthread $(CFLAGS)

all: $(EXES)

.PHONY: clean
clean:
	-rm *.out *.o

futex_wake: futex_wake.o common.o
	gcc -o futex_wake.out futex_wake.o $(COMMON_FLAGS)

futex_wait: futex_wait.o common.o
	gcc -o futex_wait.out futex_wait.o $(COMMON_FLAGS)

clock_gettime: clock_gettime.o common.o
	gcc -o clock_gettime.out clock_gettime.o $(COMMON_FLAGS)

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

process_context_switch: process_context_switch.o common.o
	gcc -o process_context_switch.out process_context_switch.o $(COMMON_FLAGS)

%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)