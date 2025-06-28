#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#define memset __memset
#define malloc __malloc
#define calloc __calloc
#define free __free

void* __memset(void*, int, size_t);
void* __malloc(size_t);
void* __calloc(size_t, size_t);
void __free(void*);


#endif // MEM_H