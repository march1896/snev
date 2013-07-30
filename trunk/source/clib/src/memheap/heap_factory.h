#ifndef _HEAP_FACTORY_H_
#define _HEAP_FACTORY_H_

#include <memheap/heap_def.h>

struct heap_wrap*  heap_wrap_spawn   (void* parent, pf_alloc alloc, pf_dealloc dealloc);
/* we can not control too much on the wrapped heap, because it's just a wrapper */
/*struct heap_wrap*  heap_wrap_spawn_v (void* parent, pf_alloc alloc, pf_dealloc dealloc, int split_threshold, int expand_size);*/
void               heap_wrap_join    (struct heap_wrap* pheap);

struct heap_llrb*  heap_llrb_spawn   (void* parent, pf_alloc alloc, pf_dealloc dealloc);
struct heap_llrb*  heap_llrb_spawn_v (void* parent, pf_alloc alloc, pf_dealloc dealloc, int split_threshold, int expand_size);
void               heap_llrb_join    (struct heap_llrb* pheap);

struct heap_buddy* heap_buddy_spawn  (void* parent, pf_alloc alloc, pf_dealloc dealloc);
struct heap_buddy* heap_buddy_spawn_v(void* parent, pf_alloc alloc, pf_dealloc dealloc, int split_threshold, int expand_size);
void               heap_buddy_join   (struct heap_buddy* pheap);

/* we can not spawn a system heap.
struct heap_sysd*  heap_sysd_spawn   (void* parent, pf_alloc alloc, pf_dealloc dealloc);
struct heap_sysd*  heap_sysd_spawn_v (void* parent, pf_alloc alloc, pf_dealloc dealloc, int split_threshold, int expand_size);
void               heap_sysd_join    (struct heap_sysd* pheap);
*/
#endif /* _HEAP_FACTORY_H_ */
