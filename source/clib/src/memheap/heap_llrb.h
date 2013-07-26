#ifndef _HEAP_LLRB_H_
#define _HEAP_LLRB_H_

#include <heap_def.h>

/* init a heap on a buffer of a given size */
heap_handle theap_init(void *buff, int size);

/* deinit a heap on a given buffer */
void theap_destroy(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* theap_alloc(heap_handle pheap, int size);

/* dealloc a buff from a heap */
void theap_dealloc(heap_handle pheap, void *buff);

/* dump debug information of the heap */
void theap_dump(heap_handle pheap);

#ifdef _MEM_DEBUG_
void theap_debug_leak(heap_handle hhdl);
void* theap_alloc_debug(heap_handle hhdl, int size, const char* file, int line);
#endif

void theap_init_global(int size);

void theap_deinit_global();

void theap_debug_global_leak();

extern heap_handle theap_global_handle;


#endif /* _HEAP_LLRB_H_ */
