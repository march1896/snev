#ifndef _MEM_HEAP_BUDDY_
#define _MEM_HEAP_BUDDY_

#include <heap_interface.h>

/* this file implement the mem_heap_interface use the buddy system malloc/free */

#ifndef MEMORY_DEBUG
heap_handle cb_heap_init_buddy	(void *buff, int size);

void	cb_heap_destroy_buddy	(heap_handle pheap);

void*	cb_heap_alloc_buddy		(heap_handle pheap, int size);

void	cb_heap_dealloc_buddy	(heap_handle pheap, void *buff);

void*	cb_heap_realloc_buddy	(heap_handle pheap, void *buff, int size);

void 	cb_heap_dump_buddy		(heap_handle pheap);

#else
heap_handle cb_heap_init_buddy_debug	(void *buff, int size);

void	cb_heap_destroy_buddy_debug		(heap_handle pheap);

void*	cb_heap_alloc_buddy_debug		(heap_handle pheap, int size, const char* file, size_t line);

void	cb_heap_dealloc_buddy_debug		(heap_handle pheap, void *buff, const char* file, size_t line);

void*	cb_heap_realloc_buddy_debug		(heap_handle pheap, void *buff, int size, const char* file, size_t line);

void 	cb_heap_dump_buddy_debug		(heap_handle pheap);
#endif // MEMORY_DEBUG

void fill_heap_operations_buddy (heap_operations* ops);

#endif // _MEM_HEAP_BUDDY_
