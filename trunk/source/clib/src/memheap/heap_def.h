#ifndef _HEAP_DEF_H_
#define _HEAP_DEF_H_

#include <cominc.h>
#define _MEM_DEBUG_

typedef void* heap_handle;

/*
 * For a heap implementation, the below methods should be implemented, since the 
 * client may pass the heap method as callbacks to manage their memory.
 */
typedef void* (*pf_alloc_c)   (heap_handle h, int size);
typedef void* (*pf_alloc_v)   (heap_handle h, int size, const char* file, int line);

typedef void (*pf_dealloc)    (heap_handle h, void* buff);

typedef void (*pf_mem_process)(void* mem);
typedef void (*pf_mem_walk)   (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_mem_walk_v) (heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param);

#define __call0(function)                          function()
#define __call1(function, p0)                      function(p0)
#define __call2(function, p0, p1)                  function(p0, p1)
#define __call3(function, p0, p1, p2)              function(p0, p1, p2)
#define __call4(function, p0, p1, p2, p3)          function(p0, p1, p2, p3)
#define __call5(function, p0, p1, p2, p3, p4)      function(p0, p1, p2, p3, p4)
#define __call6(function, p0, p1, p2, p3, p4, p5)  function(p0, p1, p2, p3, p4, p5)

/* in fact, what we need is compile time choose of function */
#define __VERBOSE_ALLOC__
#ifdef __VERBOSE_ALLOC__
#define pf_alloc pf_alloc_c
#define alloc(func, handle, size) __call2(func, handle, size)
#else
#define pf_alloc pf_alloc_v
#define alloc(func, handle, size) __call4(func, handle, size, __FILE__, __LINE__)
#endif 

struct base_heap {
	pf_alloc         __alloc;
	pf_dealloc       __dealloc;
	pf_mem_walk      __mem_walk;
	pf_mem_walk_v    __mem_walk_v;
};

/* below are the lifetime management part, it not an interface of the heap. 
 * Just like factory, the spawn/join manages the create/destroy of an object, 
 * but more than factory, it does dot creates the objects as a set, but organize 
 * them in a tree */
typedef heap_handle (*pf_heap_spawn)(heap_handle parent, pf_alloc parent_alloc, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc);
typedef void        (*pf_heap_join )(heap_handle child);


#define _USING_GLOBAL_LLRB_HEAP_
//#define _USING_GLOBAL_BUDDY_HEAP_

#endif /* _HEAP_DEF_H_ */
