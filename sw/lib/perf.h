#define perf_event_3 mhpmevent3 // 0x323
#define perf_event_4 mhpmevent4 // 0x324
#define perf_event_5 mhpmevent5 // 0x325
#define perf_event_6 mhpmevent6 // 0x326

#define perf_counter_3 mhpmcounter3 // 0xb03
#define perf_counter_4 mhpmcounter4 // 0xb04
#define perf_counter_5 mhpmcounter5 // 0xb05
#define perf_counter_6 mhpmcounter6 // 0xb06

#define CYCLES_ACTIVE 0x01

#define xstr(s) str(s)
#define str(s) #s

#define set_perf_event(perf_event, event) \
    __asm__ volatile ("csrw "xstr(perf_event)", %0" : : "r" (event))

#define set_perf_counter(perf_counter, value) \
    __asm__ volatile ("csrw "xstr(perf_counter)", %0" : : "r" (value))

#define get_perf_counter(perf_counter, return_val) \
    __asm__ ("csrr %0, "xstr(perf_counter) : "=r" (return_val))
