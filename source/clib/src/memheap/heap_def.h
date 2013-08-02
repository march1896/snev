#ifndef _HEAP_DEF_H_
#define _HEAP_DEF_H_

#include <cominc.h>
#define _MEM_DEBUG_
#ifdef _MEM_DEBUG_
#define _VERBOSE_ALLOC_DEALLOC_
#endif

/*
 * For a heap implementation, the below methods must be implemented.
 */
/* require a buff from a heap, return the buff start address.
 * return NULL if there is no enough space */
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

/* give back a buff to the heap.
 * return true if the buff is successfully deallocated or buff is NULL
 * return false if the buff is previous deallocated.  */
typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);

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

/* the below methods may or may not be implemented by a heap */


/* below defines method to traverse heap, useful for get the statistic of a heap */
/* TODO: should the block info defined here? in other words, is every heap have the same 'block' concept.
 * this should not be defined in block_com because this is a concept in common, not an implement in common*/
struct heap_blockinfo {
	/* true if this block is allocated, false if the block is free */
	bool          allocated;

	void*         block_start;
	int           block_size;

	/* this is only useful for allocated blocks */
	void*         allocable_addr;
	int           allocable_size;

	/* this is only useful if the heap carry the information */
	const char*   file;
	int           line;
};
/* pf_process_block should be provided by the one who want to walk through the heap */
typedef void (*pf_process_block)(struct heap_blockinfo* block_info, void* param);
/* pf_heap_walk should be provided by the heap implementor */
typedef void (*pf_heap_walk)    (void* pheap, pf_process_block per_block_cb, void* param);

#endif /* _HEAP_DEF_H_ */
