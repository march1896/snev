#ifndef _HEAP_WRAPPED_SYSTEM_H_
#define _HEAP_WRAPPED_SYSTEM_H_

#include <heap_def.h>
#include <util/list_link.h>

/* 
 * This is just a wrap of the system heap, which saves the allocated block in a linked list,
 * which could give more debug information than the raw system heap.
 * Of cource, use more memory to store the debug information 
 */

/* Implement the following interface one by one */
/* the below interface should be implemented, from heap_def.h
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);

typedef void (*pf_mem_process)  (void* mem);
typedef void (*pf_mem_process_v)(void* mem, void* param);
typedef void (*pf_mem_walk)   (void* pheap, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_mem_walk_v) (void* pheap, pf_mem_process_v allocated_cb, pf_mem_process_v freed_cb, void* param);
*/

struct heap_wrap {
	struct list_link allocated;
};

void  heap_wrap_init    (struct heap_wrap* h, pf_alloc __alloc, pf_dealloc __dealloc);
void  heap_wrap_deinit  (struct heap_wrap* h);

void* heap_wrap_alloc   (struct heap_wrap* h, int size);
void* heap_wrap_alloc_v (struct heap_wrap* h, int size, const char* file ,int line);

bool  heap_wrap_dealloc (struct heap_wrap* h, void* buff);
bool  heap_wrap_dealloc_v(struct heap_wrap* h, void* buff, const char* file, int line);

void  heap_wrap_walk    (struct heap_wrap* h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
void  heap_wrap_walk_v  (struct heap_wrap* h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

#endif /* _HEAP_WRAPPED_SYSTEM_H_*/
