#ifndef _HEAP_GLOBAL_H_
#define _HEAP_GLOBAL_H_

#include <memheap/heap_def.h>
#include <memheap/heap_sys.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_wrap.h>
#include <memheap/heap_buddy.h>

extern void*             __global_heap_sysdef;
extern struct heap_llrb  __global_heap_llrb;
extern struct heap_buddy __global_heap_buddy;

// #ifdef _VERBOSE_ALLOC_DEALLOC_
// #define halloc(size) alloc(heap_sysd_alloc_v, __global_heap_sysdef, size)
// #define hfree(buff)  dealloc(heap_sysd_dealloc_v, __global_heap_sysdef, buff)
// #else 
// #define halloc(size) alloc(heap_sysd_alloc_c, __global_heap_sysdef, size)
// #define hfree(buff)  dealloc(heap_sysd_dealloc_c, __global_heap_sysdef, buff)
// #endif
#define halloc(size) alloc(heap_sysd_alloc, __global_heap_sysdef, size)
#define hfree(buff)  dealloc(heap_sysd_dealloc, __global_heap_sysdef, buff)

#endif /* _HEAP_GLOBAL_H_ */
