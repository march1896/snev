#ifndef _HEAP_SYSTEM_H_
#define _HEAP_SYSTEM_H_

#include <heap_def.h>

/* init a heap on a buffer of a given size */
heap_handle sysheap_init(void *buff, int size);

/* deinit a heap on a given buffer */
void sysheap_destroy(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* sysheap_alloc(heap_handle pheap, int size);

/* dealloca a buff from a heap */
void sysheap_dealloc(heap_handle pheap, void *buff);

/* dump debug information of the heap */
void sysheap_dump(heap_handle pheap);

#ifdef _MEM_DEBUG_
void sysheap_debug_leak(heap_handle hhdl);
void* sysheap_alloc_debug(heap_handle hhdl, int size, const char* file, int line);
#endif

#endif /* _HEAP_SYSTEM_H_ */
