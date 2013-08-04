#ifndef _OBJECT_ALLOCATOR_H_
#define _OBJECT_ALLOCATOR_H_

#include <idef.h>
#include <heap_def.h>

/* 
 * by using the macros, it's not clear to view the methods supported by this object.
 * the methods are:
 * 1, allocator_join
 * 2, allocator_acquire_v/allocator_acquire_c
 * 3, allocator_release_v/allocator_release_c
 * 4, allocator_get_parent
 * 5, allocator_walk
 *
 * the 2 and 3 are methods name depends the compile macros, so macro
 * allocator_alloc   = allocator_acquire_v/allocator_acquire_c
 * allocator_dealloc = allocator_release_v/allocator_release_c
 * handles this situation, after this macro, the methods can be describes as:
 * 1, allocator_join
 * 2, allocator_alloc
 * 3, allocator_dealloc
 * 4, allocator_get_parent
 * 5, allocator_walk
 */

/* allocator methods */
extern inline void  allocator_join     (allocator o); 
#ifdef _VERBOSE_ALLOC_DEALLOC_
extern inline void* allocator_acquire_v(allocator o, int size, const char* file, int line);
extern inline bool  allocator_release_v(allocator o, void* buff, const char* file, int line);
#define allocator_acquire allocator_acquire_v
#define allocator_release allocator_release_v
#else 
extern inline void* allocator_acquire_c(allocator o, int size);
extern inline bool  allocator_release_c(allocator o, void* buff);
#define allocator_acquire allocator_acquire_c
#define allocator_release allocator_release_c
#endif
extern inline allocator allocator_get_parent(allocator o);
extern inline void      allocator_walk      (allocator o, pf_process_block per_block_cb, void* param);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define allocator_alloc(i, size) allocator_acquire_v(i, size, __FILE__, __LINE__)
#define allocator_dealloc(i, buff) allocator_release_v(i, buff, __FILE__, __LINE__)
#else 
#define allocator_alloc(i, size) allocator_acquire_c(i, size)
#define allocator_dealloc(i, buff) allocator_release_c(i, buff)
#endif

/* system default allocator, which can not be spawned or joined */
extern allocator default_allocator;

extern allocator global_llrb_allocator;

extern allocator global_buddy_allocator;

/* the factory methods */
allocator allocator_llrb_spawn (allocator parent);

allocator allocator_buddy_spawn(allocator parent);

allocator allocator_pool_spawn (allocator parent);

#endif /* _OBJECT_ALLOCATOR_H_ */
