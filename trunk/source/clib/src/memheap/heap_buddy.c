#include <heap_buddy.h>

#include <heap.h>
#include <block_common.h>

#include <list_link.h>
#include <block_link.h>
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
};

int block_data_size_from_blick(struct list_link *link) {
	struct block *pb = container_of(link, struct block, link);

	return block_com_data_size(&pb->common);
}

heap_handle bheap_init(void *buff, int size) {
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
	blink_push(&pheap->buddy[mlog2(sz)], &ib->link);

	return (heap_handle)pheap;
}

void  bheap_destroy(heap_handle hhdl) {
	struct heap *pheap = (struct heap*)hhdl;
	/*
	 * Nothing to do, since we does not allocated any memory.
	 */
}

#define SPLIT_THRETHHOLD sizeof(struct block)

#ifdef _MEM_DEBUG_
void* bheap_alloc_debug(heap_handle hhdl, int size, const char* file, int line) {
	void *buff = bheap_alloc(hhdl, size);

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

void* bheap_alloc(heap_handle hhdl, int size) {
	int bit;

	struct heap *pheap = (struct heap*)hhdl;
	struct list_link *prop = NULL;
	struct block *pb = NULL;

	for (bit = mlog2(size); bit < 32; bit ++) {
		prop = blink_find(pheap->buddy[bit], size);

		if (prop != NULL)
			break;
	}

	if (prop == NULL) {
		dbg_assert(false);
		return NULL;
	}

	blink_pop(&pheap->buddy[bit], prop);

	{
		struct block *pb = container_of(prop, struct block, link);
		struct block_c *rem = NULL;

		/* When pb is deallocated, the data field should be used as list_link */
		if (size < sizeof(struct list_link)) size = sizeof(struct list_link);

		/* Split the block, add the remain block to free container */
		rem = block_com_split(&pb->common, size, SPLIT_THRETHHOLD);
		if (rem != NULL) {
			struct block *rem_block = container_of(rem, struct block, common);

			block_com_set_free(rem, true);

			bit = mlog2(block_com_data_size(rem));
			blink_push(&pheap->buddy[bit], &rem_block->link);
		}

		block_com_set_free(&pb->common, false);
		return block_com_data(&pb->common);
	}
}

void  bheap_dealloc(heap_handle hhdl, void *buff) {
	struct heap *pheap = (struct heap*)hhdl;
	struct block_c *pbc = block_com_from_data(buff);
	struct block *pb = container_of(pbc, struct block, common);
	struct block_c *prev = block_com_prev_adj(pbc);
	struct block_c *next = block_com_next_adj(pbc);
	int bit;

	if (block_com_valid(prev) && block_com_free(prev)) {
		if (block_com_valid(next) && block_com_free(next)) {
			struct block *b_prev = container_of(prev, struct block, common);
			struct block *b_next = container_of(next, struct block, common);
			dbg_assert((void*)b_prev == (void*)prev);
			dbg_assert((void*)b_next == (void*)next);

			bit = block_com_data_size(prev);
			blink_pop(&pheap->buddy[bit], &b_prev->link);
			bit = block_com_data_size(next);
			blink_pop(&pheap->buddy[bit], &b_next->link);

			block_com_merge(prev, next);

			bit = block_com_data_size(prev);
			blink_push(&pheap->buddy[bit], &b_prev->link);

			block_com_set_free(prev, true);
		}
		else {
			struct block *b_prev = container_of(prev, struct block, common);
			dbg_assert((void*)b_prev == (void*)prev);

			bit = block_com_data_size(prev);
			blink_pop(&pheap->buddy[bit], &b_prev->link);

			block_com_merge(prev, pbc);

			bit = block_com_data_size(prev);
			blink_push(&pheap->buddy[bit], &b_prev->link);

			block_com_set_free(prev, true);
		}
	}
	else if (block_com_valid(next) && block_com_free(next)) {
		struct block *b_next = container_of(next, struct block, common);
		dbg_assert((void*)b_next == (void*)next);

		bit = block_com_data_size(next);
		blink_pop(&pheap->buddy[bit], &b_next->link);

		block_com_merge(pbc, next);

		bit = block_com_data_size(pbc);
		blink_push(&pheap->buddy[bit], &pb->link);

		block_com_set_free(pbc, true);
	}
	else {
		bit = block_com_data_size(pbc);
		blink_push(&pheap->buddy[bit], &pb->link);

		block_com_set_free(pbc, true);
	}
}

void bheap_dump(heap_handle hhdl) {
}

#ifdef _MEM_DEBUG_
#include <stdio.h>
void bheap_debug_leak(heap_handle hhdl) {
	struct heap *pheap = (struct heap*)hhdl;
	struct block_c *pbc = pheap->pfirst;

	while (block_com_valid(pbc)) {
		if (!block_com_free(pbc)) {
			printf("%s %d\n", pbc->file, pbc->line);
		}
		pbc = block_com_next_adj(pbc);
	}
}

heap_handle _global_buddy_heap;

void bheap_debug_global_leak() {
	bheap_debug_leak(_global_buddy_heap);
}
#endif 

void bheap_init_global(int size) {
	void *buff = malloc(size);
	_global_buddy_heap = bheap_init(buff, size);
}

void bheap_deinit_global() {
	struct heap *pheap = (struct heap *)_global_buddy_heap;
	void *buff = pheap->pbuff;
	bheap_destroy(_global_buddy_heap);
	free(buff);
}
