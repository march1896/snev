#ifndef _HEAP_DEF_H_
#define _HEAP_DEF_H_

#include <cominc.h>
#define _MEM_DEBUG_

typedef void* heap_handle;

/*
 * For a heap implementation, the below methods should be implemented, since the 
 * client may pass the heap method as callbacks to manage their memory.
 */
typedef void* (*pf_alloc)     (heap_handle h, int size);
typedef void* (*pf_alloc_v)   (heap_handle h, int size, const char* file, int line);

typedef void (*pf_dealloc)    (heap_handle h, void* buff);

typedef void (*pf_mem_process)(void* mem);
typedef void (*pf_heap_walk)  (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_heap_walk_v)(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

typedef void (*pf_heap_init)  (heap_handle h, pf_alloc mem_increase, pf_alloc_v mem_increase_v, pf_dealloc mem_decrease, heap_handle parent);
typedef void (*pf_heap_deinit)(heap_handle h);

typedef heap_handle (*pf_heap_spawn)(heap_handle parent, pf_alloc parent_alloc, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc);
typedef void        (*pf_heap_join )(heap_handle child);


#define _USING_GLOBAL_LLRB_HEAP_
//#define _USING_GLOBAL_BUDDY_HEAP_

#endif /* _HEAP_DEF_H_ */
