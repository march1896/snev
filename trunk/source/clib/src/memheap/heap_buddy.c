#include <heap_buddy.h>

#include <block_link.h>

#include <util/list_link.h>
#include <util/math.h>

int block_data_size_from_blick(struct list_link *link) {
	struct heap_buddy_block *pb = container_of(link, struct heap_buddy_block, link);

	return block_com_data_size(&pb->common);
}

static void heap_buddy_expand_memory(struct heap_buddy* pheap, int expand_size) {
	struct block_c_pool* n_blk_pool = (struct block_c_pool*)
		alloc(pheap->__alloc, pheap->__parent, sizeof(struct block_c_pool));

	void* n_mem_begin = 
		alloc(pheap->__alloc, pheap->__parent, expand_size);

	dbg_assert(n_mem_begin != NULL);
	{
		/* initialize the new memory */
		struct block_c *sent_first = NULL;
		struct block_c *sent_last  = NULL;
		struct block_c *init_block = NULL;
		void* n_mem_end   = (void*)((char*)n_mem_begin + expand_size);

		init_block = block_com_make_sentinels(
				n_mem_begin, n_mem_end, &sent_first, &sent_last);
		block_com_set_free(init_block, true);

		n_blk_pool->memory = n_mem_begin;
		n_blk_pool->size   = expand_size;
		n_blk_pool->bc_first      = init_block;
		n_blk_pool->bc_front_sent = sent_first;
		n_blk_pool->bc_end_sent   = sent_last;
	}
	
	/* link the new memory into memory list */
	list_insert_back(&pheap->memlist, &n_blk_pool->link);

	/* insert the new block into the heap */
	{
		struct heap_buddy_block *n_block = 
			container_of(n_blk_pool->bc_first, struct heap_buddy_block, common);
		int sz = block_com_data_size(n_blk_pool->bc_first);

		blink_push(&pheap->buddy[mlog2(sz)], &n_block->link);
	}
}

void heap_buddy_init(struct heap_buddy* pheap, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	heap_buddy_init_v(pheap, __parent, __alloc, __dealloc, 
			sizeof(struct heap_buddy_block),
			HEAP_MINIMUM_EXPAND_SIZE);
}

void heap_buddy_init_v(struct heap_buddy* pheap, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int __split_threshold, int __expand_size) {
	int i;

	pheap->__parent  = __parent;
	pheap->__alloc   = __alloc;
	pheap->__dealloc = __dealloc;

	for (i = 0; i < BUDDY_COUNT; i ++) 
		list_init(&pheap->buddy[i]);

	list_init(&pheap->memlist);

	/* sizeof(struct heap_buddy_block) is the minimum threshold, because after 
	 * splitting we should still the block in 'free list' */
	pheap->split_threshold = max(sizeof(struct heap_buddy_block), __split_threshold);
	pheap->expand_size      = max(HEAP_MINIMUM_EXPAND_SIZE, __expand_size);
}

static void block_c_pool_dispose(struct list_link* link, void* param) {
	struct block_c_pool* blk_pool = 
		container_of(link, struct block_c_pool, link);
	struct heap_buddy* pheap = (struct heap_buddy*)param;

	/* first delete the memory hold by this node */
	dealloc(pheap->__dealloc, pheap->__parent, blk_pool->memory);

	/* second delete the memory node itself */
	dealloc(pheap->__dealloc, pheap->__parent, blk_pool);
}

void heap_buddy_deinit(struct heap_buddy* pheap) {
	/* clear the memory hold by this heap */
	list_foreach_v(&pheap->memlist, block_c_pool_dispose, (void*)pheap);
}

static void* heap_buddy_alloc_try(struct heap_buddy* pheap, int size) {
	int bit = 0;

	struct list_link *prop = NULL;
	struct heap_buddy_block *pb = NULL;

	for (bit = mlog2(size); bit < BUDDY_COUNT; bit ++) {
		prop = blink_find(&pheap->buddy[bit], size);

		if (prop != NULL)
			break;
	}

	if (prop == NULL) {
		/* we dont have enough space for this block */
		return NULL;
	}

	blink_pop(&pheap->buddy[bit], prop);

	{
		struct heap_buddy_block *pb = container_of(prop, struct heap_buddy_block, link);
		struct block_c *rem = NULL;

		/* When pb is deallocated, the data field should be used as list_link */
		if (size < sizeof(struct list_link)) 
			size = sizeof(struct list_link);

		/* Split the block, add the remain block to free container */
		rem = block_com_split(&pb->common, size, pheap->split_threshold);
		if (rem != NULL) {
			struct heap_buddy_block *rem_block = container_of(rem, struct heap_buddy_block, common);

			block_com_set_free(rem, true);

			bit = mlog2(block_com_data_size(rem));
			blink_push(&pheap->buddy[bit], &rem_block->link);
		}

		block_com_set_free(&pb->common, false);
		return block_com_data(&pb->common);
	}

	return NULL;
}

void* heap_buddy_alloc_c(struct heap_buddy* pheap, int size) {
	void* mem = heap_buddy_alloc_try(pheap, size);

	if (mem == NULL) {
		/* we dont have enough space to alloc this size */
		int expand_size = pheap->expand_size;
		if (expand_size < 2 * size) {
			/* if the expand size is not big enough for we need */
			expand_size = 2 * size;
		}

		heap_buddy_expand_memory(pheap, expand_size);

		/* second try */
		mem = heap_buddy_alloc_try(pheap, size);
		dbg_assert(mem != NULL);
	}

	return mem;
}

void* heap_buddy_alloc_v(struct heap_buddy* pheap, int size, const char* file, int line) {
	void* buff = heap_buddy_alloc_c(pheap, size);

	if (buff == NULL)
		return NULL;

	{
		struct block_c *pbc = (struct block_c*)block_com_from_data(buff);

		block_com_debug_set_fileline(pbc, file, line);
	}

	return buff;
}

bool heap_buddy_dealloc_c (struct heap_buddy* pheap, void* buff) {
	struct block_c *pbc    = block_com_from_data(buff);
	struct heap_buddy_block *pb = container_of(pbc, struct heap_buddy_block, common);
	struct block_c *prev   = block_com_prev_adj(pbc);
	struct block_c *next   = block_com_next_adj(pbc);
	int bit = 0;

	if (buff == NULL) {
		return true;
	}

	if (!block_com_valid(pbc)) {
		/* dealloc an invalid block, break it. */
		dbg_assert(false);
	}

	if (block_com_free(pbc)) {
		/* we are try to dealloc the buff twice, handle this situation. */
		return false;
	}

	if (block_com_valid(prev) && block_com_free(prev)) {
		if (block_com_valid(next) && block_com_free(next)) {
			struct heap_buddy_block *b_prev = container_of(prev, struct heap_buddy_block, common);
			struct heap_buddy_block *b_next = container_of(next, struct heap_buddy_block, common);
			dbg_assert((void*)b_prev == (void*)prev);
			dbg_assert((void*)b_next == (void*)next);

			bit = mlog2(block_com_data_size(prev));
			blink_pop(&pheap->buddy[bit], &b_prev->link);
			bit = mlog2(block_com_data_size(next));
			blink_pop(&pheap->buddy[bit], &b_next->link);

			block_com_merge(prev, next);

			bit = mlog2(block_com_data_size(prev));
			blink_push(&pheap->buddy[bit], &b_prev->link);

			block_com_set_free(prev, true);
		}
		else {
			struct heap_buddy_block *b_prev = container_of(prev, struct heap_buddy_block, common);
			dbg_assert((void*)b_prev == (void*)prev);

			bit = mlog2(block_com_data_size(prev));
			blink_pop(&pheap->buddy[bit], &b_prev->link);

			block_com_merge(prev, pbc);

			bit = mlog2(block_com_data_size(prev));
			blink_push(&pheap->buddy[bit], &b_prev->link);

			block_com_set_free(prev, true);
		}
	}
	else if (block_com_valid(next) && block_com_free(next)) {
		struct heap_buddy_block *b_next = container_of(next, struct heap_buddy_block, common);
		dbg_assert((void*)b_next == (void*)next);

		bit = mlog2(block_com_data_size(next));
		blink_pop(&pheap->buddy[bit], &b_next->link);

		block_com_merge(pbc, next);

		bit = mlog2(block_com_data_size(pbc));
		blink_push(&pheap->buddy[bit], &pb->link);

		block_com_set_free(pbc, true);
	}
	else {
		bit = mlog2(block_com_data_size(pbc));
		blink_push(&pheap->buddy[bit], &pb->link);

		block_com_set_free(pbc, true);
	}

	return true;
}

bool heap_buddy_dealloc_v (struct heap_buddy* pheap, void* buff, const char* file, int line) {
	bool res = heap_buddy_dealloc_c(pheap, buff);
	struct block_c *pbc = (struct block_c*)block_com_from_data(buff);

	if (buff == NULL) {
		return true;
	}

	if (res == false) {
		/* the address is deleted twice */
		/*
		char* __file;
		int __line;
		file = block_com_debug_get_fileline(pbc, &line);
		printf("The address %8X is dealloced twice\n");
		printf("Last time is:\n");
		printf("%s : (%d)\n", __file, __line);
		printf("This time is:\n");
		printf("%s : (%d)\n", file, line);
		*/
		/* TODO: handle the message */
		dbg_assert(false);
	}

	block_com_debug_set_fileline(pbc, file, line);

	return res;
}
