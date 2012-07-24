#ifndef _MEM_HEAP_SYSTEM_H_
#define _MEM_HEAP_SYSTEM_H_

#include <heap_interface.h>

/* this file implement heap_interface by using system default malloc/free */

/* callbacks without debug information */
heap_handle cb_heap_init_sys(void *buff, int size);

void	cb_heap_destroy_sys	(heap_handle pheap);

void*	cb_heap_alloc_sys	(heap_handle pheap, int size, const char* file, size_t line);

void	cb_heap_dealloc_sys	(heap_handle pheap, void *buff, const char* file, size_t line);

void*	cb_heap_realloc_sys	(heap_handle pheap, void *buff, int size, const char* file, size_t line);

void 	cb_heap_dump_sys	(heap_handle pheap);

/* call backs with debug information */
heap_handle cb_heap_init_sys_debug	(void *buff, int size);

void	cb_heap_destroy_sys_debug	(heap_handle pheap);

void*	cb_heap_alloc_sys_debug		(heap_handle pheap, int size, const char* file, size_t line);

void	cb_heap_dealloc_sys_debug	(heap_handle pheap, void *buff, const char* file, size_t line);

void*	cb_heap_realloc_sys_debug	(heap_handle pheap, void *buff, int size, const char* file, size_t line);

void 	cb_heap_dump_sys_debug		(heap_handle pheap);

/* this function will fill ops with corresponding callbacks according to 
 * MEMORY_DEBUG macro */
void	fill_heap_operations_sys	(heap_operations* ops);

#endif // _MEM_HEAP_SYSTEM_H_
