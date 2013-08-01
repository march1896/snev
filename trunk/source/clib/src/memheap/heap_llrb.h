#ifndef _HEAP_LLRB_H_
#define _HEAP_LLRB_H_

#include <heap_def.h>

#include <util/llrb_link.h>
#include <util/list_link.h>

#include <block_common.h>
/*
 * This module implements a heap by using llrb as the free list container
 */

/* heap_llrb_block describes a free block, 
 * block_c describes a common block, no matter its allocated or free*/
struct heap_llrb_block {
	struct block_c     common;
	//struct block_c_clean xx;

	struct llrb_link   link;
};

struct heap_llrb {
	void*              __parent;
	/* the inner alloc/dealloc callback to manage the inner usage of this heap */
	pf_alloc           __alloc;
	pf_dealloc         __dealloc;

	/* point to the single free list */
	struct llrb_link*  llrb_root; 

	/* this list holds a list of memory which is used by this heap */
	struct list_link   memlist;

	unsigned int       split_threthhold;
	unsigned int       expand_size;
};


/* the below interface should be implemented, from heap_def.h
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);
*/

void* heap_llrb_alloc_c (struct heap_llrb* pheap, int size);
void* heap_llrb_alloc_v (struct heap_llrb* pheap, int size, const char* file, int line);

bool heap_llrb_dealloc_c(struct heap_llrb* pheap, void* buff);
bool heap_llrb_dealloc_v(struct heap_llrb* pheap, void* buff, const char* file, int line);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define heap_llrb_alloc   heap_llrb_alloc_v
#define heap_llrb_dealloc heap_llrb_dealloc_v
#else 
#define heap_llrb_alloc   heap_llrb_alloc_c
#define heap_llrb_dealloc heap_llrb_dealloc_c
#endif

/* get block info part */
void heap_llrb_get_blockinfo(void* mem_addr, /* out */ struct heap_blockinfo* info);

/* traverse part */
void heap_llrb_walk     (struct heap_llrb* pheap, pf_process_block per_block_cb, void* param);

void heap_llrb_init     (struct heap_llrb* pheap, void* parent, pf_alloc __alloc, pf_dealloc __dealloc);
void heap_llrb_init_v   (struct heap_llrb* pheap, void* parent, pf_alloc __alloc, pf_dealloc __dealloc, int __split_threadhold, int __expand_size);
void heap_llrb_deinit   (struct heap_llrb* pheap);

#endif /* _HEAP_LLRB_H_ */
