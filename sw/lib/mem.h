#ifndef MEM_H
#define MEM_H

#include <stddef.h>

void* memcpy(void* restrict, const void* restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
void* malloc(size_t);
void* calloc(size_t, size_t);
void free(void*);
void _reset_heap();

#endif // MEM_H