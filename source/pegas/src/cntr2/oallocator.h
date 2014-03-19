#ifndef _OBJECT_ALLOCATOR_H_
#define _OBJECT_ALLOCATOR_H_

#include <cntr2/idef.h>
#include <memheap/heap_def.h>

/* 
 * the allocator object, allocator is not typical object, it does not have most of the 
 * method in iobject.h, it only responds to join method plus heap operation method.
 *
 * 1, allocator_join
 * 2, allocator_acquire
 * 3, allocator_release
 * 4, allocator_get_parent
 * 5, allocator_walk
 */

/* 
 * allocator implements the 'heap interface' implicitly. recall the heap definition in heap_def.h
 * it has two method pf_alloc, pf_dealloc and a heap_handle.
 * allocator has allocator_acquire, allocator_release and allocator, we could pass the above 
 * three functions to any where that needs a pf_alloc, pf_dealloc and a heap handler.
 */

/* allocator methods */
extern inline void  allocator_join   (allocator o); 
#ifdef _VERBOSE_ALLOC_DEALLOC_
extern inline void* allocator_acquire(allocator o, int size, const char* file, int line);
extern inline bool  allocator_release(allocator o, void* buff, const char* file, int line);
#else 
extern inline void* allocator_acquire(allocator o, int size);
extern inline bool  allocator_release(allocator o, void* buff);
#endif
extern inline allocator allocator_get_parent(allocator o);
extern inline void      allocator_walk      (allocator o, pf_process_block per_block_cb, void* param);

/* codes below is only useful for allocator implementors */
typedef void   (*pf_allocator_join)       (_object* o);
#ifdef _VERBOSE_ALLOC_DEALLOC_
typedef void*  (*pf_allocator_acquire)   (_object* o, int size, const char* file, int line);
typedef bool   (*pf_allocator_release)   (_object* o, void* buff, const char* file, int line);
#else 
typedef void*  (*pf_allocator_acquire)  (_object* o, int size);
typedef bool   (*pf_allocator_release)  (_object* o, void* buff);
#endif
typedef _object* (*pf_allocator_get_parent) (_object* o);
typedef void   (*pf_allocator_walk)       (_object* o, pf_process_block per_block_cb, void* param);


struct allocator_vtable {
	/* heap doest not contains destroy method */
	pf_allocator_join           __join;

	pf_allocator_acquire        __acquire;
	pf_allocator_release        __release;

	pf_allocator_get_parent     __get_parent;
	pf_allocator_walk           __heap_walk;
};

#define allocator_alloc(o, size)   alloc(allocator_acquire, o, size)
#define allocator_dealloc(o, buff) dealloc(allocator_release, o, buff)

/* system default allocator, which can not be spawned or joined */
extern allocator __global_sysd_allocator;
extern allocator __global_llrb_allocator;
extern allocator __global_buddy_allocator;

extern allocator __global_default_allocator;

/* the factory methods */
allocator allocator_llrb_spawn (allocator parent);

allocator allocator_buddy_spawn(allocator parent);

allocator allocator_spool_spawn(allocator parent);

allocator allocator_mpool_spawn(allocator parent, int max_diff_types);

#endif /* _OBJECT_ALLOCATOR_H_ */
