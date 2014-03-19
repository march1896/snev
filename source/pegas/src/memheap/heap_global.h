#ifndef _HEAP_GLOBAL_H_
#define _HEAP_GLOBAL_H_

#include <memheap/heap_def.h>
#include <memheap/heap_sys.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_buddy.h>

extern void*      __global_sysd_heap;
extern pf_alloc   __global_sysd_alloc;
extern pf_dealloc __global_sysd_dealloc;

extern void*      __global_llrb_heap;
extern pf_alloc   __global_llrb_alloc;
extern pf_dealloc __global_llrb_dealloc;

extern void*      __global_buddy_heap;
extern pf_alloc   __global_buddy_alloc;
extern pf_dealloc __global_buddy_dealloc;

extern void*      __global_default_heap;
extern pf_alloc   __global_default_alloc;
extern pf_dealloc __global_default_dealloc;

/* please don't include system headers after this file */
#ifdef halloc
#define sys_malloc halloc
#undef halloc
#endif 
#ifdef hfree
#define sys_free hfree
#undef hfree
#endif

#define halloc(size) alloc(__global_default_alloc, __global_default_heap, size)
#define hfree(buff)  dealloc(__global_default_dealloc, __global_default_heap, buff)

/*
#define malloc(size) alloc(heap_sysd_alloc, __global_heap_sysd, size)
#define free(buff)   dealloc(heap_sysd_dealloc, __global_heap_sysd, buff)

#define malloc(size) alloc(heap_llrb_alloc, (struct heap_llrb*)__global_heap_llrb, size)
#define free(buff)   dealloc(heap_llrb_dealloc, (struct heap_llrb*)__global_heap_llrb, buff)
*/

extern heap_info g_heap;
extern heap_info g_sysheap;
extern heap_info g_llrbheap;
extern heap_info g_buddyheap;

#endif /* _HEAP_GLOBAL_H_ */
