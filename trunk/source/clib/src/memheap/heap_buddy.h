#ifndef _HEAP_BUDDY_H_
#define _HEAP_BUDDY_H_

#include <heap_def.h>
#include <block_common.h>

#include <util/list_link.h>

struct heap_buddy_block {
	/* common part of block */
	struct block_c 		common;		

	/* members to maintain double linked list */
	struct list_link    link;
};

#define BUDDY_COUNT 32
struct heap_buddy {
	void*              __parent;
	/* the inner alloc/dealloc callback to manage the inner usage of this heap */
	pf_alloc           __parent_alloc;
	pf_dealloc         __parent_dealloc;

	/* point to the single free list */
	struct list_link   buddy[BUDDY_COUNT]; 

	struct list_link   memlist;

	unsigned int       split_threshold;
	unsigned int       expand_size;
};

/* the below interface should be implemented, from heap_def.h
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);
*/

void* heap_buddy_alloc_c  (struct heap_buddy* pheap, int size);
void* heap_buddy_alloc_v  (struct heap_buddy* pheap, int size, const char* file, int line);

bool heap_buddy_dealloc_c (struct heap_buddy* pheap, void* buff);
bool heap_buddy_dealloc_v (struct heap_buddy* pheap, void* buff, const char* file, int line);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define heap_buddy_alloc   heap_buddy_alloc_v
#define heap_buddy_dealloc heap_buddy_dealloc_v
#else 
#define heap_buddy_alloc   heap_buddy_alloc_c
#define heap_buddy_dealloc heap_buddy_dealloc_c
#endif

/* traverse part */
void heap_buddy_walk      (struct heap_buddy* pheap, pf_process_block per_block_cb, void* param);

void heap_buddy_init      (struct heap_buddy* pheap, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc);
void heap_buddy_init_v    (struct heap_buddy* pheap, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int __split_threadhold, int __expand_size);
void heap_buddy_deinit    (struct heap_buddy* pheap);

#endif /* _HEAP_BUDDY_H_ */
