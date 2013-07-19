#ifndef _HEAP_BUDDY_H_
#define _HEAP_BUDDY_H_

#include <heap_def.h>

/* init a heap on a buffer of a given size */
heap_handle bheap_init(void *buff, int size);

//heap_handle bheap_init(pf_mem_increase cb_inc, pf_mem_decrease cn_dec);

/* deinit a heap on a given buffer */
void bheap_destroy(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* bheap_alloc(heap_handle pheap, int size);

/* dealloca a buff from a heap */
void bheap_dealloc(heap_handle pheap, void *buff);

/* dump debug information of the heap */
void bheap_dump(heap_handle pheap);

#ifdef _MEM_DEBUG_
void bheap_debug_leak(heap_handle hhdl);
void* bheap_alloc_debug(heap_handle hhdl, int size, const char* file, int line);
#endif

void bheap_init_global(int size);

void bheap_deinit_global();

void bheap_debug_global_leak();

#endif /* _HEAP_BUDDY_H_ */
