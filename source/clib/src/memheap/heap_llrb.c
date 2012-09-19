#include <heap.h>
#include <block_common.h>
#include <llrb_link.h>

/*
 * This file implements a heap by using llrb as the freelist container
 */

struct block {
	struct block_c 	 common;

	struct llrb_link link;
};

struct heap {
	/* point to the single free list */
	struct llrb_link *root; 

	/* point to the start of the heap buffer. */
	void*			pbuff; 

	/* total size of the heap, include heap header, block pool and all align gaps */
	unsigned int 	size;

	unsigned int 	flag;

	unsigned int 	error;
};

static int block_comp(const struct llrb_link *l, const struct llrb_link *r) {
	struct block *plb, *prb;
	unsigned lsz, rsz;
	plb = container_of(l, struct block, link);
	prb = container_of(r, struct block, link);

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
	struct block *pb = container_of(c, struct block, link);
	int sz = block_com_data_size(&pb->common);

	if (c == NULL) 
		return NULL;

	if (sz < size) 
		return llrb_find(c->right, size);
	else if (c->left == NULL)
		return c;
	else 
		return llrb_find(c->left, size);

	return NULL;
}

heap_handle heap_init(void *buff, int size) {
	struct heap *pheap = (struct heap*)buff;

	void *block_start = (char*)buff + sizeof(struct heap);
	void *block_end = (char*)buff + size;

	struct block_c *sent_first;
	struct block_c *sent_last;
	struct block_c *init_block;
	struct block *ib;

	init_block = block_com_make_sentinels(
			block_start, block_end, &sent_first, &sent_last);

	pheap->root = NULL;
	pheap->pbuff = buff;
	pheap->size = size;
	pheap->flag = 0;
	pheap->error = 0;

	block_com_set_free(init_block, true);

	ib = container_of(init_block, struct block, common);
	pheap->root = llrb_insert(pheap->root, &ib->link, block_comp);

	return (heap_handle)pheap;
}

void  heap_destroy(heap_handle hhdl) {
	struct heap *pheap = (struct heap*)hhdl;
	/*
	 * Nothing to do, since we does not allocated any memory.
	 */
}

#define SPLIT_THRETHHOLD sizeof(struct block)

void* heap_alloc(heap_handle hhdl, int size, const char* file, size_t line) {
	struct heap *pheap = (struct heap*)hhdl;

	struct llrb_link *prop = llrb_find(pheap->root, size);
	struct block *pb = container_of(prop, struct block, link);
	struct block_c *rem = NULL;

	if (prop == NULL) return NULL;

	/* Remove the block from free container */
	pheap->root = llrb_remove(pheap->root, prop, block_comp);

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

void  heap_dealloc(heap_handle hhdl, void *buff, const char* file, size_t line) {
	struct heap *pheap = (struct heap*)hhdl;
	struct block_c *pbc = block_com_from_data(buff);
	struct block *pb = container_of(pbc, struct block, link);
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

void* heap_realloc(heap_handle hhdl, void *buff, int size, const char* file, size_t line) {
	/* TODO */
	return NULL;
}

void heap_dump(heap_handle hhdl) {
	/* TODO */
}
