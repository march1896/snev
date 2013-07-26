#ifndef _HEAP_WRAPPED_SYSTEM_H_
#define _HEAP_WRAPPED_SYSTEM_H_

#include <heap_def.h>

/* 
 * This is just a wrap of the system heap, which saves the allocated block in a linked list,
 * which could give more debug information than the raw system heap.
 * Of cource, use more memory to store the debug information 
 */

/* Implement the following interface one by one */
/*
typedef void* (*pf_alloc)     (heap_handle h, int size);
typedef void* (*pf_alloc_v)   (heap_handle h, int size, const char* file, int line);

typedef void (*pf_dealloc)    (heap_handle h, void* buff);

typedef void (*pf_mem_process)(void* mem);
typedef void (*pf_heap_walk)  (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_heap_walk_v)(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

typedef void (*pf_heap_init)  (heap_handle h, pf_alloc mem_increase, pf_alloc_v mem_increase_v, pf_dealloc mem_decrease, heap_handle parent);
typedef void (*pf_heap_deinit)(heap_handle h);

typedef heap_handle (*pf_heap_spawn  )(heap_handle parent, pf_alloc   parent_alloc,   pf_dealloc parent_dealloc);
typedef heap_handle (*pf_heap_spawn_v)(heap_handle parent, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc);
typedef void        (*pf_heap_join   )(heap_handle child);
*/

void* wheap_alloc   (heap_handle h, int size);
void* wheap_alloc_v (heap_handle h, int size, const char* file ,int line);

void  wheap_dealloc (heap_handle h, void* buff);

void  wheap_walk    (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
void  wheap_walk_v  (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

void  wheap_init    (heap_handle h, pf_alloc mem_increase, pf_dealloc mem_decrease, heap_handle parent);
void  wheap_init_v  (heap_handle h, pf_alloc_v mem_increase_v, pf_dealloc mem_decrease, heap_handle parent);

void  wheap_deinit  (heap_handle h);

heap_handle wheap_spawn  (heap_handle parent, pf_alloc   parent_alloc,   pf_dealloc parent_dealloc);
heap_handle wheap_spawn_v(heap_handle parent, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc);
void        wheap_join   (heap_handle child);

#endif /* _HEAP_WRAPPED_SYSTEM_H_*/
