#include <block_common.h>
#include <llrb_link.h>

static int block_comp(const struct llrb_link *l, const struct llrb_link *r) {
	struct heap_llrb_block *plb, *prb;
	unsigned lsz, rsz;
	plb = container_of(l, struct heap_llrb_block, link);
	prb = container_of(r, struct heap_llrb_block, link);

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
	struct heap_llrb_block *pb;
	int sz;
	
	if (c == NULL) 
		return NULL;

	pb = container_of(c, struct heap_llrb_block, link);
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

void theap_init(struct heap_llrb* pheap) {
	void *block_start = (char*)buff + sizeof(struct heap_llrb);
	void *block_end = (char*)buff + size;

	struct block_c *sent_first;
	struct block_c *sent_last;
	struct block_c *init_block;
	struct heap_llrb_block *ib;

	init_block = block_com_make_sentinels(
			block_start, block_end, &sent_first, &sent_last);

	pheap->root = NULL;
	pheap->pbuff = buff;
	pheap->pfirst = init_block;
	pheap->size = size;
	pheap->flag = 0;
	pheap->error = 0;

	block_com_set_free(init_block, true);

	ib = container_of(init_block, struct heap_llrb_block, common);
	pheap->root = llrb_insert(pheap->root, &ib->link, block_comp);
}

void  theap_destroy(struct heap_llrb* pheap) {
	/*
	 * TODO:
	 * Nothing to do, since we does not allocated any memory.
	 */
}

#define SPLIT_THRETHHOLD sizeof(struct heap_llrb_block)

void* theap_alloc(struct heap_llrb* pheap, int size) {
	struct llrb_link *prop = llrb_find(pheap->root, size);
	struct heap_llrb_block *pb = container_of(prop, struct heap_llrb_block, link);
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
		struct heap_llrb_block *rem_block = container_of(rem, struct heap_llrb_block, common);

		block_com_set_free(rem, true);

		pheap->root = llrb_insert(pheap->root, &rem_block->link, block_comp);
	}

	block_com_set_free(&pb->common, false);
	return block_com_data(&pb->common);
}

#ifdef _MEM_DEBUG_
void* theap_alloc_debug(struct heap_llrb* pheap, int size, const char* file, int line) {
	void *buff = theap_alloc(pheap, size);

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

void  theap_dealloc(struct heap_llrb* pheap, void *buff) {
	struct block_c *pbc = block_com_from_data(buff);
	struct heap_llrb_block *pb = container_of(pbc, struct heap_llrb_block, common);
	struct block_c *prev = block_com_prev_adj(pbc);
	struct block_c *next = block_com_next_adj(pbc);

	if (block_com_valid(prev) && block_com_free(prev)) {
		if (block_com_valid(next) && block_com_free(next)) {
			struct heap_llrb_block *b_prev = container_of(prev, struct heap_llrb_block, common);
			struct heap_llrb_block *b_next = container_of(next, struct heap_llrb_block, common);
			dbg_assert((void*)b_prev == (void*)prev);
			dbg_assert((void*)b_next == (void*)next);

			pheap->root = llrb_remove(pheap->root, &b_prev->link, block_comp);
			pheap->root = llrb_remove(pheap->root, &b_next->link, block_comp);

			block_com_merge(prev, next);
			pheap->root = llrb_insert(pheap->root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
		else {
			struct heap_llrb_block *b_prev = container_of(prev, struct heap_llrb_block, common);
			dbg_assert((void*)b_prev == (void*)prev);

			pheap->root = llrb_remove(pheap->root, &b_prev->link, block_comp);

			block_com_merge(prev, pbc);
			pheap->root = llrb_insert(pheap->root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
	}
	else if (block_com_valid(next) && block_com_free(next)) {
		struct heap_llrb_block *b_next = container_of(next, struct heap_llrb_block, common);
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

void theap_dump(struct heap_llrb* pheap) {
}

#ifdef _MEM_DEBUG_
#include <stdio.h>
void theap_debug_leak(struct heap_llrb* pheap) {
	struct block_c *pbc = pheap->pfirst;

	while (block_com_valid(pbc)) {
		if (!block_com_free(pbc)) {
			printf("%s %d\n", pbc->file, pbc->line);
		}
		pbc = block_com_next_adj(pbc);
	}
}
#endif _MEM_DEBUG_