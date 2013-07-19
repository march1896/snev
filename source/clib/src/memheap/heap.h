#ifndef _HEAP_H_
#define _HEAP_H_

#include <heap_def.h>

/* TODO:
   1, the heap does not support alignment, alignment is important in SIMD.
   2, the heap can only allocate a huge block of memory from system memory, but can not increasingly.
      this is easy to fix, since we could more than one block of memory could be easily linked into a free list or tree.
*/

#ifdef _USING_GLOBAL_LLRB_HEAP_
#include <heap_llrb.h>
extern heap_handle _global_llrb_heap;
#ifdef _MEM_DEBUG_
#define halloc(size) theap_alloc_debug(_global_llrb_heap, size, __FILE__, __LINE__)
#define heap_debug_global_leak() theap_debug_global_leak()
#else 
#define halloc(size) theap_alloc(_global_llrb_heap, size)
#endif

#define hfree(buff) theap_dealloc(_global_llrb_heap, buff)
#define heap_init_global(sz) theap_init_global(sz)
#define heap_deinit_global() theap_deinit_global()

#elif defined(_USING_GLOBAL_BUDDY_HEAP_)
#include <heap_buddy.h>
extern heap_handle _global_buddy_heap;
#ifdef _MEM_DEBUG_
#define halloc(size) bheap_alloc_debug(_global_buddy_heap, size, __FILE__, __LINE__)
#define heap_debug_global_leak() bheap_debug_global_leak()
#else 
#define halloc(size) bheap_alloc(_global_buddy_heap, size)
#endif

#define hfree(buff) bheap_dealloc(_global_buddy_heap, buff)
#define heap_init_global(sz) bheap_init_global(sz)
#define heap_deinit_global() bheap_deinit_global()

#else 

#define halloc malloc
#define hfree free
#define heap_init_global(sz)
#define heap_deinit_global()
#ifdef _MEM_DEBUG_
#define heap_debug_global_leak()
#endif

#endif

#endif /* _HEAP_H_ */
