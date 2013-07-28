#ifndef _HEAP_LLRB_H_
#define _HEAP_LLRB_H_

/*
 * This module implements a heap by using llrb as the free list container
 */
struct heap_llrb_block {
	struct block_c 	 common;

	struct llrb_link link;
};

struct heap_llrb {
	/* the inner alloc/dealloc callback to manage the inner usage of this heap */
	pf_alloc        __alloc;
	pf_dealloc      __dealloc;

	/* point to the single free list */
	struct llrb_link *root; 

	/* point to the start of the heap buffer. */
	/* TODO: we should change the pbuff to r_dlist like list, so we could 
	 * increase and decrease the memory dynamically */
	void*			pbuff; 

	struct block_c  *pfirst;

	/* total size of the heap, include heap header, block pool and all align gaps */
	unsigned int 	size;

	unsigned int 	flag;

	unsigned int 	error;
};


void theap_init(heap_llrb* pheap, pf_alloc __alloc, pf_dealloc __dealloc);

/* deinit a heap on a given buffer */
void theap_destroy(struct heap_llrb* pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* theap_alloc(struct heap_llrb* pheap, int size);

/* dealloc a buff from a heap */
void theap_dealloc(struct heap_llrb* pheap, void *buff);

/* dump debug information of the heap */
void theap_dump(struct heap_llrb* pheap);

#ifdef _MEM_DEBUG_
void theap_debug_leak(struct heap_llrb* hhdl);
void* theap_alloc_debug(struct heap_llrb* hhdl, int size, const char* file, int line);
#endif

void theap_init_global(int size);

void theap_deinit_global();

void theap_debug_global_leak();


#endif /* _HEAP_LLRB_H_ */
