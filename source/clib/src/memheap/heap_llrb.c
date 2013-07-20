#include <heap.h>
#include <block_common.h>
#include <llrb_link.h>

/*
 * This file implements a heap by using llrb as the freelist container
 */

struct block_llrb {
	struct block_c 	 common;

	struct llrb_link link;
};

struct heap_llrb {
	/* point to the single free list */
	struct llrb_link *root; 

	/* point to the start of the heap buffer. */
	void*			pbuff; 

	struct block_c  *pfirst;

	/* total size of the heap, include heap header, block pool and all align gaps */
	unsigned int 	size;

	unsigned int 	flag;

	unsigned int 	error;
};

heap_handle _global_llrb_heap;

static int block_comp(const struct llrb_link *l, const struct llrb_link *r) {
	struct block_llrb *plb, *prb;
	unsigned lsz, rsz;
	plb = container_of(l, struct block_llrb, link);
	prb = container_of(r, struct block_llrb, link);

	lsz = block_com_size(&plb->common);
	rsz = block_com_size(&prb->common);

	if (lsz < rsz) return -1;
	else if (lsz == rsz) return 0;
	else return 1;
}

/* 
 * Find a proper block in the llrb, here we try to find the smallest 
 * block who can hold greater or equal than size.
 */
struct llrb_link *llrb_find(struct llrb_link *c, int size) {
	struct block_llrb *pb;
	int sz;
	
	if (c == NULL) 
		return NULL;

	pb = container_of(c, struct block_llrb, link);
	sz = block_com_data_size(&pb->common);

	if (sz < size) 
		return llrb_find(c->right, size);
	else if (c->left == NULL)
		return c;
	else {
		struct llrb_link *found = llrb_find(c->left, size);
		if (found) 
			return found;
		else return c;
	}

	return NULL;
}

heap_handle theap_init(void *buff, int size) {
	struct heap_llrb *pheap = (struct heap_llrb*)buff;

	void *block_start = (char*)buff + sizeof(struct heap_llrb);
	void *block_end = (char*)buff + size;

	struct block_c *sent_first;
	struct block_c *sent_last;
	struct block_c *init_block;
	struct block_llrb *ib;

	init_block = block_com_make_sentinels(
			block_start, block_end, &sent_first, &sent_last);

	pheap->root = NULL;
	pheap->pbuff = buff;
	pheap->pfirst = init_block;
	pheap->size = size;
	pheap->flag = 0;
	pheap->error = 0;

	block_com_set_free(init_block, true);

	ib = container_of(init_block, struct block_llrb, common);
	pheap->root = llrb_insert(pheap->root, &ib->link, block_comp);

	return (heap_handle)pheap;
}

void  theap_destroy(heap_handle hhdl) {
	struct heap_llrb *pheap = (struct heap_llrb*)hhdl;
	/*
	 * Nothing to do, since we does not allocated any memory.
	 */
}

#define SPLIT_THRETHHOLD sizeof(struct block_llrb)

void* theap_alloc(heap_handle hhdl, int size) {
	struct heap_llrb *pheap = (struct heap_llrb*)hhdl;

	struct llrb_link *prop = llrb_find(pheap->root, size);
	struct block_llrb *pb = container_of(prop, struct block_llrb, link);
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
		struct block_llrb *rem_block = container_of(rem, struct block_llrb, common);

		block_com_set_free(rem, true);

		pheap->root = llrb_insert(pheap->root, &rem_block->link, block_comp);
	}

	block_com_set_free(&pb->common, false);
	return block_com_data(&pb->common);
}

#ifdef _MEM_DEBUG_
void* theap_alloc_debug(heap_handle hhdl, int size, const char* file, int line) {
	void *buff = theap_alloc(hhdl, size);

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

void  theap_dealloc(heap_handle hhdl, void *buff) {
	struct heap_llrb *pheap = (struct heap_llrb*)hhdl;
	struct block_c *pbc = block_com_from_data(buff);
	struct block_llrb *pb = container_of(pbc, struct block_llrb, common);
	struct block_c *prev = block_com_prev_adj(pbc);
	struct block_c *next = block_com_next_adj(pbc);

	if (block_com_valid(prev) && block_com_free(prev)) {
		if (block_com_valid(next) && block_com_free(next)) {
			struct block_llrb *b_prev = container_of(prev, struct block_llrb, common);
			struct block_llrb *b_next = container_of(next, struct block_llrb, common);
			dbg_assert((void*)b_prev == (void*)prev);
			dbg_assert((void*)b_next == (void*)next);

			pheap->root = llrb_remove(pheap->root, &b_prev->link, block_comp);
			pheap->root = llrb_remove(pheap->root, &b_next->link, block_comp);

			block_com_merge(prev, next);
			pheap->root = llrb_insert(pheap->root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
		else {
			struct block_llrb *b_prev = container_of(prev, struct block_llrb, common);
			dbg_assert((void*)b_prev == (void*)prev);

			pheap->root = llrb_remove(pheap->root, &b_prev->link, block_comp);

			block_com_merge(prev, pbc);
			pheap->root = llrb_insert(pheap->root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
	}
	else if (block_com_valid(next) && block_com_free(next)) {
		struct block_llrb *b_next = container_of(next, struct block_llrb, common);
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

void theap_dump(heap_handle hhdl) {
}

#ifdef _MEM_DEBUG_
#include <stdio.h>
void theap_debug_leak(heap_handle hhdl) {
	struct heap_llrb *pheap = (struct heap_llrb*)hhdl;
	struct block_c *pbc = pheap->pfirst;

	while (block_com_valid(pbc)) {
		if (!block_com_free(pbc)) {
			printf("%s %d\n", pbc->file, pbc->line);
		}
		pbc = block_com_next_adj(pbc);
	}
}

void theap_debug_global_leak() {
	theap_debug_leak(_global_llrb_heap);
}
#endif 

void theap_init_global(int size) {
	void *buff = malloc(size);
	_global_llrb_heap = theap_init(buff, size);
}

void theap_deinit_global() {
	struct heap_llrb *pheap = (struct heap_llrb *)_global_llrb_heap;
	void *buff = pheap->pbuff;
	theap_destroy(_global_llrb_heap);
	free(buff);
}