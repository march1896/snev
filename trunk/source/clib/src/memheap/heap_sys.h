#ifndef _HEAP_SYSTEM_H_
#define _HEAP_SYSTEM_H_

#include <heap_def.h>

/* Implement the following interface one by one */
/*
typedef void* (*pf_alloc)     (heap_handle h, int size);
typedef void* (*pf_alloc_v)   (heap_handle h, int size, const char* file, int line);

typedef void (*pf_dealloc)    (heap_handle h, void* buff);

typedef void (*pf_mem_process)(void* mem);
typedef void (*pf_heap_walk)  (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_heap_walk_v)(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

typedef heap_handle (*pf_heap_spawn)(heap_handle parent, pf_alloc parent_alloc, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc);
typedef void        (*pf_heap_join )(heap_handle child);
*/

void* sheap_alloc   (heap_handle h, int size);
void* sheap_alloc_v (heap_handle h, int size, const char* file ,int line);

void  sheap_dealloc (heap_handle h, void* buff);

void  sheap_walk    (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
void  sheap_walk_v  (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

/* 
 * we can not spawn a system heap, system heap is the parent of all heaps we spawn.
 */

#ifdef _MEM_DEBUG_
#endif

#endif /* _HEAP_SYSTEM_H_ */
