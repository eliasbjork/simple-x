/*
 * This is an extremely silly heap allocator. The malloc implementation simply
 * allocates after the previous allocation (4 byte aligned) unless the stack is
 * encountered, and free does nothing. If we end up running out of memory in
 * simplex, I might consider actually writing free :)
 */

#include <stdint.h>
#include "mem.h"


extern unsigned int _heap_start;
static uintptr_t heap_head = (uintptr_t)&_heap_start;


static inline void _align_heap_head() {
    int rem = heap_head % 4;
    if (rem != 0) {
        heap_head += 4 - rem;
    }
}


void _reset_heap() {
    heap_head = (uintptr_t)&_heap_start;
}


void* memcpy(void* restrict dest, const void* restrict src, size_t count) {
    uint8_t* end = (uint8_t*)src + count;
    uint8_t* p = (uint8_t*)src;
    uint8_t* q = (uint8_t*)dest;

    while (p < end) {
        *q++ = *p++;
    }

    return dest;
}


void* memmove(void* dest, const void* src, size_t count) {
    uint8_t* p = (uint8_t*)src;
    uint8_t* p_end = p + count;
    uint8_t* q = (uint8_t*)dest;

    if (dest < src) {
        // forward copy
        while (p < p_end) {
            *q++ = *p++;
        }
    } else if (dest > src) {
        // backward copy
        while (p_end > p) {
            *(--q) = *(--p);
        }
    }

    return dest;
}


void* memset(void* dest, int ch, size_t count) {
    uint8_t fill_byte = (uint8_t)ch;
    uint8_t* end = (uint8_t*)dest + count;
    uint8_t* p = (uint8_t*)dest;

    while (p < end) {
        *p++ = fill_byte;
    }

    return dest;
}


void* malloc(size_t size) {
    uintptr_t sp;
    __asm__ volatile ("mv %0, sp" : "=r"(sp));

    if (heap_head + size >= sp) {
        return NULL;
    }

    void* p = (void*)heap_head;

    heap_head += size;
    _align_heap_head();

    return p;
}


void* calloc(size_t num, size_t size) {
    void* p = malloc(num * size);

    if (p != NULL) {
        memset(p, 0, num * size);
    }

    return p;
}


void free(void*) {}