CODEGEN=-mcpu=cortex-a53 -mfpu=neon -mfloat-abi=hard -mapcs-frame
CFLAGS=-O0 -std=gnu11 -Wall $(CODEGEN)
EXES=clock_gettime hpet_clock_resolutions procedure_call system_call_overhead \
	 task_creation for_loop futex_wake futex_wait thread_context_switch \
	 process_context_switch memory_bandwidth page_fault_access file_cache
COMMON_FLAGS=common.o -lm -lrt -lcpufreq -pthread $(CFLAGS)

OUTS=$(patsubst %,%.out,$(EXES))

all: $(OUTS)

.PHONY: clean
clean:
	-rm *.o
	-rm *.out

%.out: %.o common.o
	gcc -o $@ $< $(COMMON_FLAGS)

ram_access_time: ram_access_time.o common.o
	gcc -o ram_access_time.out ram_access_time.o $(COMMON_FLAGS)

rand: rand.o common.o
	gcc -o rand.out rand.o $(COMMON_FLAGS)

%.o: %.c
	gcc -c -o $@ $^ $(CFLAGS)