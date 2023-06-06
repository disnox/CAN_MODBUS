#ifndef __ALLOC_H_
#define __ALLOC_H_

#define USE_RT_MALLOC

#if defined(USE_ALLOC)
/*定义alloc.c内存管理数组大小*/
#define AllocArraySize (16*1024)
extern void xa_free_m( void *ap );
extern void* xa_malloc_m( unsigned nbytes );
extern void *xa_realloc(void *mem_address, unsigned int newsize);

#define xa_free(p) {xa_free_m(p); p = 0;}
//#define xa_malloc(n) xa_malloc_m(n, __FUNCTION__, __LINE__);
#define xa_malloc(n) xa_malloc_m(n);


#elif defined(USE_RT_MALLOC)

#include "rt_malloc.h"
/*定义alloc.c内存管理数组大小*/
#define AllocArraySize (1*1024)

#if defined(AllocArraySize)
void rt_mem_init(void);
#define xa_mem_init() rt_mem_init()
#else
#define xa_mem_init(begin_addr, end_addr) rt_system_heap_init(begin_addr, end_addr)
#endif

#define xa_free(rmem) {rt_free(rmem); rmem = 0;}
#define xa_malloc(size) rt_malloc(size)
#define xa_realloc(rmem, newsize) rt_realloc(rmem, newsize)
#define xa_calloc(count, size) rt_calloc(count, size)

#define xa_mem_info(total, used, max_used) rt_memory_info(total, used, max_used)

#else
#include <stdlib.h>
#define xa_free(rmem) {free(rmem); rmem = 0;}
#define xa_malloc(size) malloc(size)
#define xa_realloc(rmem, newsize) realloc(rmem, newsize)
#define xa_calloc(count, size) calloc(count, size)
#endif

#endif

