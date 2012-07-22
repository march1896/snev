#ifndef _MEM_HEAP_BUDDY_
#define _MEM_HEAP_BUDDY_

#include <heap_interface.h>

/* this file implement the mem_heap_interface use the system default malloc/free */

heap_handle cb_heap_init_buddy	(void *buff, int size);

void	cb_heap_destroy_buddy	(heap_handle pheap);

void*	cb_heap_alloc_buddy		(heap_handle pheap, int size);

void	cb_heap_dealloc_buddy	(heap_handle pheap, void *buff);

void	cb_heap_realloc_buddy	(heap_handle pheap, void *buff, int size);

#endif // _MEM_HEAP_BUDDY_
