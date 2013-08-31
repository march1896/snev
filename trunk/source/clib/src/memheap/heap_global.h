#ifndef _HEAP_GLOBAL_H_
#define _HEAP_GLOBAL_H_

#include <memheap/heap_def.h>

#include <memheap/heap_sys.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_buddy.h>

extern void*              __global_heap_sysd;
extern struct heap_llrb*  __global_heap_llrb;
extern struct heap_buddy* __global_heap_buddy;

#define malloc(size) alloc(heap_buddy_alloc, __global_heap_buddy, size)
#define free(buff)   dealloc(heap_buddy_dealloc, __global_heap_buddy, buff)

/*
#define malloc(size) alloc(heap_sysd_alloc, __global_heap_sysd, size)
#define free(buff)   dealloc(heap_sysd_dealloc, __global_heap_sysd, buff)

#define malloc(size) alloc(heap_llrb_alloc, __global_heap_llrb, size)
#define free(buff)   dealloc(heap_llrb_dealloc, __global_heap_llrb, buff)
*/

#endif /* _HEAP_GLOBAL_H_ */
