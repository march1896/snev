#ifndef _MEM_HEAP_SYSTEM_H_
#define _MEM_HEAP_SYSTEM_H_

#include <heap_interface.h>

/* this file implement the mem_heap_interface use the system default malloc/free */

heap_handle cb_heap_init_sys(void *buff, int size);

void	cb_heap_destroy_sys	(heap_handle pheap);

void*	cb_heap_alloc_sys	(heap_handle pheap, int size);

void	cb_heap_dealloc_sys	(heap_handle pheap, void *buff);

void	cb_heap_realloc_sys	(heap_handle pheap, void *buff, int size);

#endif // _MEM_HEAP_SYSTEM_H_
