#ifndef _HEAP_DEF_H_
#define _HEAP_DEF_H_

#include <cominc.h>
#define _MEM_DEBUG_
// #define _VERBOSE_ALLOC_DEALLOC_

/*
 * For a heap implementation, the below methods should be implemented, since the 
 * client may pass the heap method as callbacks to manage their memory.
 */
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);

typedef void (*pf_mem_process) (void* mem);
typedef void (*pf_mem_process_v)(void* mem, void* param);

typedef void (*pf_mem_walk)   (void* pheap, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_mem_walk_v) (void* pheap, pf_mem_process_v allocated_cb, pf_mem_process_v freed_cb, void* param);

/* TODO: move these macros to somewhere */
#define __call0(function)                          function()
#define __call1(function, p0)                      function(p0)
#define __call2(function, p0, p1)                  function(p0, p1)
#define __call3(function, p0, p1, p2)              function(p0, p1, p2)
#define __call4(function, p0, p1, p2, p3)          function(p0, p1, p2, p3)
#define __call5(function, p0, p1, p2, p3, p4)      function(p0, p1, p2, p3, p4)
#define __call6(function, p0, p1, p2, p3, p4, p5)  function(p0, p1, p2, p3, p4, p5)

/* in fact, what we need is compile time choose of function */
#ifdef _VERBOSE_ALLOC_DEALLOC_
#define pf_alloc    pf_alloc_v
#define pf_dealloc  pf_dealloc_v

#define alloc(func, handle, size)  __call4(func, handle, size, __FILE__, __LINE__)
#define dealloc(func, handle, buff)  __call4(func, handle, buff, __FILE__, __LINE__)

#else
#define pf_alloc    pf_alloc_c
#define pf_dealloc  pf_dealloc_c

#define alloc(func, handle, size)  __call2(func, handle, size)
#define dealloc(func, handle, buff)  __call2(func, handle, buff)
#endif 

#define HEAP_MINIMUM_EXPAND_SIZE (4*1024*1024)

#endif /* _HEAP_DEF_H_ */
