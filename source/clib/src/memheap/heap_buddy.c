#include <block_common.h>
#include <list_link.h>
#include <bit_math.h>

struct block {
	/* common part of block */
	struct block_c 		common;		

	/* members to maintain double linked list */
	struct list_link    link;
};

#define BN 32
struct heap {
	/* point to the single free list */
	struct list_link *buddy[BN]; 

	/* point to the start of the heap buffer. */
	void*			pbuff; 

	struct block_c  *pfirst;

	/* total size of the heap, include heap header, block pool and all align gaps */
	unsigned int 	size;

	unsigned int 	flag;

	unsigned int 	error;
}

heap_handle heap_init(void *buff, int size) {
	struct heap *pheap = (struct heap*)buff;

	void *block_start = (char*)buff + sizeof(struct heap);
	void *block_end = (char*)buff + size;

	struct block_c *sent_first;
	struct block_c *sent_last;
	struct block_c *init_block;
	struct block *ib;
	int i, sz;

	init_block = block_com_make_sentinels(
			block_start, block_end, &sent_first, &sent_last);

	for (i = 0; i < BN; i ++)
		pheap->buddy[i] = NULL;
	pheap->pbuff = buff;
	pheap->pfirst = init_block;
	pheap->size = size;
	pheap->flag = 0;
	pheap->error = 0;

	block_com_set_free(init_block, true);
	ib = container_of(init_block, struct block, common);

	sz = block_com_data_size(init_block);
	pheap->buddy[mlog2(sz)] = /* TODO: we should add extended list_link ops which support different insert/find strategy */

	pheap->root = llrb_insert(pheap->root, &ib->link, block_comp);

	return (heap_handle)pheap;
}

static inline struct list_link *_find_buddy(struct heap *pheap, struct block_c *pbc) {
	int sz = block_com_data_size(pbc);

	return pheap->buddy[mlog2(sz)];
}

void  heap_destroy(heap_handle hhdl) {
	struct heap *pheap = (struct heap*)hhdl;
	/*
	 * Nothing to do, since we does not allocated any memory.
	 */
}

#define SPLIT_THRETHHOLD sizeof(struct block)

#ifdef _MEM_DEBUG_
void* heap_alloc_debug(heap_handle hhdl, int size, const char* file, int line) {
	void *buff = heap_alloc(hhdl, size);

	if (buff == NULL)
		return NULL;

	{
		struct block_c *pbc = (struct block_c*)block_com_from_data(buff);

		pbc->file = file;
		pbc->line = line;
	}

	return buff;
}
#endif

void* heap_alloc(heap_handle hhdl, int size) {
	struct heap *pheap = (struct heap*)hhdl;

	struct llrb_link *prop = llrb_find(pheap->root, size);
	struct block *pb = container_of(prop, struct block, link);
	struct block_c *rem = NULL;

	if (prop == NULL) {
		dbg_assert(false);
		return NULL;
	}

	/* Remove the block from free container */
	pheap->root = llrb_remove(pheap->root, prop, block_comp);

	/* When pb is deallocated, the data field should be used as llrb_link */
	if (size < sizeof(struct llrb_link)) size = sizeof(struct llrb_link);

	/* Split the block, add the remain block to free container */
	rem = block_com_split(&pb->common, size, SPLIT_THRETHHOLD);
	if (rem != NULL) {
		struct block *rem_block = container_of(rem, struct block, common);

		block_com_set_free(rem, true);

		pheap->root = llrb_insert(pheap->root, &rem_block->link, block_comp);
	}

	block_com_set_free(&pb->common, false);
	return block_com_data(&pb->common);
}

void  heap_dealloc(heap_handle hhdl, void *buff) {
	struct heap *pheap = (struct heap*)hhdl;
	struct block_c *pbc = block_com_from_data(buff);
	struct block *pb = container_of(pbc, struct block, common);
	struct block_c *prev = block_com_prev_adj(pbc);
	struct block_c *next = block_com_next_adj(pbc);

	if (block_com_valid(prev) && block_com_free(prev)) {
		if (block_com_valid(next) && block_com_free(next)) {
			struct block *b_prev = container_of(prev, struct block, common);
			struct block *b_next = container_of(next, struct block, common);
			dbg_assert((void*)b_prev == (void*)prev);
			dbg_assert((void*)b_next == (void*)next);

			pheap->root = llrb_remove(pheap->root, &b_prev->link, block_comp);
			pheap->root = llrb_remove(pheap->root, &b_next->link, block_comp);

			block_com_merge(prev, next);
			pheap->root = llrb_insert(pheap->root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
		else {
			struct block *b_prev = container_of(prev, struct block, common);
			dbg_assert((void*)b_prev == (void*)prev);

			pheap->root = llrb_remove(pheap->root, &b_prev->link, block_comp);

			block_com_merge(prev, pbc);
			pheap->root = llrb_insert(pheap->root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
	}
	else if (block_com_valid(next) && block_com_free(next)) {
		struct block *b_next = container_of(next, struct block, common);
		dbg_assert((void*)b_next == (void*)next);

		pheap->root = llrb_remove(pheap->root, &b_next->link, block_comp);

		block_com_merge(pbc, next);
		pheap->root = llrb_insert(pheap->root, &pb->link, block_comp);

		block_com_set_free(pbc, true);
	}
	else {
		pheap->root = llrb_insert(pheap->root, &pb->link, block_comp);

		block_com_set_free(pbc, true);
	}
}

void heap_dump(heap_handle hhdl) {
}

#ifdef _MEM_DEBUG_
#include <stdio.h>
void heap_debug_leak(heap_handle hhdl) {
	struct heap *pheap = (struct heap*)hhdl;
	struct block_c *pbc = pheap->pfirst;

	while (block_com_valid(pbc)) {
		if (!block_com_free(pbc)) {
			printf("%s %d\n", pbc->file, pbc->line);
		}
		pbc = block_com_next_adj(pbc);
	}
}

void heap_debug_global_leak() {
	heap_debug_leak(_global_llrb_heap);
}
#endif 

void heap_init_global(int size) {
	void *buff = malloc(size);
	_global_llrb_heap = heap_init(buff, size);
}

void heap_deinit_global() {
	struct heap *pheap = (struct heap *)_global_llrb_heap;
	void *buff = pheap->pbuff;
	heap_destroy(_global_llrb_heap);
	free(buff);
}


#if 0

typedef struct block_dll_t block_dll;

/*
 * double linked list(dll) has several way to manage blocks, 
 * 1, sort free blocks in increasing order.
 * 2, sort free blocks in decreasing order.
 * 3, keep free blocks in random order.
 *
 * for all of the managing method list above, we have two ways 
 * to find a proper block that can carry enough data.
 * 1, find free block first fit.
 * 2, find free block best fit.
 *
 * from implementation point of view, below mothod should be provided.
 * 1, block_dll_find_ff (find a proper block first fit)
 * 2, block_dll_find_bf (find a proper block best fit)
 * 3, block_dll_pop (pop/erase a block from the dll)
 * 4, block_dll_push (push/insert a block into the dll)
 */

typedef enum {
	E_BDLLFT_FIRSTFIT,
	E_BDLLFT_BESTFIT,
}block_dll_find_type ;

typedef enum {
	E_BDLLST_INC,
	E_BDLLST_DEC,
	E_BDLLST_RND
}block_dll_sort_type ;

/**
 * @brief find a proper block in free list according size requirement
 *
 * @param pphead address of the header of free list
 * @param req required data size
 *
 * @return 
 */
typedef block_dll* (*pf_block_dll_find)(block_dll** pphead, unsigned int req);

/**
 * @brief pop a block from freelist, this function just change double linked list 
 *   related properties, it does not change any block_com related properties.
 *   so, after you pop the block from free list, you must manually set block_com's
 *   free property.
 *
 * @param pphead address of the header of free list
 * @param pbd the block_dll to be poped
 *
 */
typedef void       (*pf_block_dll_pop) (block_dll** pphead, block_dll* pbd);

/**
 * @brief push a block into freelist, this function just change double linked 
 *   list related properties, it does not change any block_com related properties.
 *   so, after you push a block into free list, you must manully set block_com's 
 *   free property.
 *
 * @param pphead address of the header of free list
 * @param pbd the block_dll to be pushed
 *
 */
typedef void       (*pf_block_dll_push)(block_dll** pphead, block_dll* pbd);

typedef struct block_dll_operations_h {
	pf_block_dll_find 	find;
	pf_block_dll_pop	pop;
	pf_block_dll_push   push;
} block_dll_ops;

void block_dll_make_operations(block_dll_ops* ops, block_dll_find_type ft, block_dll_sort_type st);

static block_dll* block_dllinc_find_ff (block_dll** pphead, unsigned int req);
static block_dll* block_dllinc_find_bf (block_dll** pphead, unsigned int req);
static void       block_dllinc_pop     (block_dll** pphead, block_dll* pbd);
static void       block_dllinc_push    (block_dll** pphead, block_dll* pbd);

static block_dll* block_dlldec_find_ff (block_dll** pphead, unsigned int req);
static block_dll* block_dlldec_find_bf (block_dll** pphead, unsigned int req);
static void       block_dlldec_pop     (block_dll** pphead, block_dll* pbd);
static void       block_dlldec_push    (block_dll** pphead, block_dll* pbd);

static block_dll* block_dllrnd_find_ff (block_dll** pphead, unsigned int req);
static block_dll* block_dllrnd_find_bf (block_dll** pphead, unsigned int req);
static void       block_dllrnd_pop     (block_dll** pphead, block_dll* pbd);
static void       block_dllrnd_push    (block_dll** pphead, block_dll* pbd);

/* common functions that for all three management. */
static void inline block_c* block_dll_to_com(block_dll* pbd) {
	dbg_assert(&(pbd->bc)== (block_c*)pbd);
	return (block_c*)pbd;
}

static void block_dll_pop_common(block_dll** pphead, block_dll* pbd) {
	/* dbg_assert(block_com_free(block_dll_to_com(pbd))); */

	block_dll* prev = pbd->prev_free;
	block_dll* next = pbd->next_free;

	if (prev) prev->next_free = next;
	if (next) next->prev_free = prev;

	if (*pphead == pbd) *pphead = next;

	block_com_markallocated(block_dll_to_com(pbd));
}

static block_dll* block_dll_find_ff_common(block** pphead, unsigned int req) {
	unsigned int bsize = req + sizeof(block_c);
	block_dll* itr = *pphead;
	while (itr != NULL) {
		if (block_com_size(block_dll_to_com(itr)) >= bsize) return itr;

		itr = itr->next_free;
	}

	return NULL;
}

static void block_dll_link_into_freelist(block** pphead, block* prev, block* pbd, block* next) {
	// no matter if prev/next is NULL, points to them
	pbd->prev_free = prev;
	pbd->next_free = next;

	if (*pphead == NULL) {
		// link list contains no element.
		dbg_assert(prev == NULL && next == NULL);

		*pphead = pbd;
	}
	else {
		// contains at least one element
		if (prev == NULL) {
			dbg_assert(next != NULL);
			// push at the very front of freelist
			next->prev_free = pbd;
			
			*pphead = pbd;
		}
		else {
			// in the middle or at the end of the list
			dbg_assert(!next || next->prev_free == prev);

			prev->next = pbd;
			if (next) next->prev_free = pbd;
		}
	}

	return;
}

/**********************************************************************************/

block_dll* block_dllinc_find_ff(block_dll** pphead, unsigned int req) {
	unsigned int bsize = req + sizeof(block_c);
	block_dll* itr = *pphead;
	while (itr != NULL) {
		if (block_com_size(block_dll_to_com(itr)) >= bsize) return itr;

		itr = itr->next_free;
	}

	return NULL;
}

block_dll* block_dllinc_find_bf(block_dll** pphead, unsigned int req) {
	return block_dllinc_find_ff(pphead, req);
}

void block_dllinc_pop(block_dll** pphead, block_dll* pbd) {
	block_dll_pop_common(pphead, pbd);
}

void block_dllinc_push(block_dll** pphead, block_dll* pbd) {
	// we should insert the block in increasing order.
	dbg_assert(!block_com_isfree(block_dll_to_com(pbd)));

	block_dll* next = *pphead;
	block_dll* prev = NULL;
	unsigned int cur_size = block_com_size(block_dll_to_com(pbd));
	// immediate break since free list is in increasing order
	while (next != NULL) {
		if (block_com_size(block_dll_to_com(next)) >= cur_size) break;

		prev = next;
		next = next->next_free;
	}

	block_dll_link_into_freelist(pphead, prev, pbd, next);

	block_com_markfree(block_dll_to_com(pbd));

	return;
}


block_dll* block_dlldec_find_ff(block_dll** pphead, unsigned int req) {
	return block_dll_find_ff_common(pphead, req);
}

block_dll* block_dlldec_find_bf(block_dll** pphead, unsigned int req) {
	unsigned int bsize = req + sizeof(block_c);

	block_dll* next = *pphead;
	block_dll* prev = NULL;

	while (next != NULL) {
		if (block_com_size(block_dll_to_com(next)) < bsize) break;

		prev = next;
		next = next->next_free;
	}

	return prev;
}

void block_dlldec_pop(block_dll** pphead, block_dll* pbd) {
	block_dll_pop_common(pphead, pbd);
}

void block_dlldec_push(block_dll** pphead, block_dll* pbd) {
	dbg_assert(!block_com_isfree(block_dll_to_com(pbd)));

	unsigned int bsize = block_com_size(pbd->size);
	block_dll* next = *pphead;
	block_dll* prev = NULL;

	while (next != NULL) {
		if (block_com_size(block_dll_to_com(next)) <= bsize) break;

		prev = next;
		next = next->next_free;
	}

	block_dll_link_into_freelist(pphead, prev, pbd, next);

	block_com_markfree(block_dll_to_com(pbd));

	return;
}


block_dll* block_dllrnd_find_ff(block_dll** pphead, unsigned int req) {
	return block_dll_find_ff_common(pphead, req);
}

block_dll* block_dllrnd_find_bf(block_dll** pphead, unsigned int req) {
	unsigned int bsize = req + sizeof(block_c);

	block_dll* itr = *pphead;
	block_dll* best = NULL;

	while (itr != NULL) {
		if (block_com_size(block_dll_to_com(itr)) >= bsize) {
			if (best == NULL) best = itr;
			else if (block_com_size(block_dll_to_com(itr)) < 
					 block_com_size(block_dll_to_com(best))) best = itr;
		}

		itr = itr->next_free;
	}

	return best;
}

void block_dllrnd_pop(block_dll** pphead, block_dll* pbd) {
	block_dll_pop_common(pphead, pbd);
	return;
}

void block_dllrnd_push(block_dll** pphead, block_dll* pbd) {
	// simply push at the very front
	block_dll_link_into_freelist(pphead, NULL, pbd, *pphead);

	block_com_markfree(block_dll_to_com(pbd));

	return;
}

void block_dll_make_operations(block_dll_ops* ops, block_dll_find_type ft, block_dll_sort_type st) {
	if (st == E_BDLLST_INC) {
		ops->push = block_dllinc_push;
		ops->pop  = block_dllinc_pop;
		if (ft == E_BDLLFT_FIRSTFIT)     ops->find = block_dllinc_find_ff;
		else if (ft == E_BDLLFT_BESTFIT) ops->find = block_dllinc_find_bf;
	}
	else if (st == E_BDLLST_DEC) {
		ops->push = block_dlldec_push;
		ops->pop  = block_dlldec_pop;
		if (ft == E_BDLLFT_FIRSTFIT)     ops->find = block_dlldec_find_ff;
		else if (ft == E_BDLLFT_BESTFIT) ops->find = block_dlldec_find_bf;
	}
	else {
		ops->push = block_dllrnd_push;
		ops->pop  = block_dllrnd_pop;
		if (ft == E_BDLLFT_FIRSTFIT)     ops->find = block_dllrnd_find_ff;
		else if (ft == E_BDLLFT_BESTFIT) ops->find = block_dllrnd_find_bf;
	}
}

#endif
