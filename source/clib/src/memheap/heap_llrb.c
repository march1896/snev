#include <heap_llrb.h>
#include <util/math.h>

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
static struct llrb_link *llrb_link_findbysize(struct llrb_link *c, int size) {
	struct heap_llrb_block *pb;
	int sz;
	
	if (c == NULL) 
		return NULL;

	pb = container_of(c, struct heap_llrb_block, link);
	sz = block_com_data_size(&pb->common);

	if (sz < size) 
		return llrb_link_findbysize(c->right, size);
	else if (c->left == NULL)
		return c;
	else {
		struct llrb_link *found = llrb_link_findbysize(c->left, size);
		if (found) 
			return found;
		else return c;
	}

	return NULL;
}

static void heap_llrb_expand_memory(struct heap_llrb* pheap, int expand_size) {
	struct block_c_pool* n_block_pool = (struct block_c_pool*)
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

		n_block_pool->memory = n_mem_begin;
		n_block_pool->size   = expand_size;
		n_block_pool->bc_first      = init_block;
		n_block_pool->bc_front_sent = sent_first;
		n_block_pool->bc_end_sent   = sent_last;

		block_com_set_free(init_block, true);
	}
	
	/* link the new memory into memory list */
	list_insert_back(&pheap->memlist, &n_block_pool->link);

	/* insert the new block into the heap */
	{
		struct heap_llrb_block *n_block = NULL;
		n_block = container_of(n_block_pool->bc_first, struct heap_llrb_block, common);

		pheap->llrb_root = llrb_insert(pheap->llrb_root, &n_block->link, block_comp);
	}
}

void heap_llrb_init(struct heap_llrb* pheap, void* parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	heap_llrb_init_v(pheap, parent, __alloc, __dealloc, 
			sizeof(struct heap_llrb_block),
			HEAP_MINIMUM_EXPAND_SIZE);
}

void heap_llrb_init_v(struct heap_llrb* pheap, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int __split_threshold, int __expand_size) {
	pheap->__parent  = __parent;
	pheap->__alloc   = __alloc;
	pheap->__dealloc = __dealloc;

	pheap->llrb_root = NULL;
	list_init(&pheap->memlist);

	/* sizeof(struct heap_llrb_block) is the minimum threthhold, because after 
	 * splitting we should still the block in 'free list' */
	pheap->split_threthhold = max(sizeof(struct heap_llrb_block), __split_threshold);
	pheap->expand_size      = max(HEAP_MINIMUM_EXPAND_SIZE, __expand_size);
}

static void block_c_pool_dispose(struct list_link* link, void* param) {
	struct block_c_pool* blk_pool = 
		container_of(link, struct block_c_pool, link);
	struct heap_llrb* pheap = (struct heap_llrb*)param;

	/* first delete the memory hold by this node */
	dealloc(pheap->__dealloc, pheap->__parent, blk_pool->memory);

	/* second delete the memory node itself */
	dealloc(pheap->__dealloc, pheap->__parent, blk_pool);
}

void heap_llrb_deinit(struct heap_llrb* pheap) {
	/* clear the memory hold by this heap */
	list_foreach_v(&pheap->memlist, block_c_pool_dispose, (void*)pheap);
}

static void* heap_llrb_alloc_try(struct heap_llrb* pheap, int size) {
	struct llrb_link* alink = llrb_link_findbysize(pheap->llrb_root, size);
	struct heap_llrb_block* pb = container_of(alink, struct heap_llrb_block, link);
	struct block_c* rem = NULL;
	
	if (alink == NULL) {
		/* we don't have enough space for this size */
		return NULL;
	}

	/* Remove the block from free container */
	pheap->llrb_root = llrb_remove(pheap->llrb_root, alink, block_comp);

	/* When pb is deallocated, the data field should be used as llrb_link */
	if (size < sizeof(struct llrb_link)) size = sizeof(struct llrb_link);

	/* Split the block, add the remain block to free container */
	rem = block_com_split(&pb->common, size, pheap->split_threthhold);
	if (rem != NULL) {
		struct heap_llrb_block *rem_block = container_of(rem, struct heap_llrb_block, common);

		block_com_set_free(rem, true);

		pheap->llrb_root = llrb_insert(pheap->llrb_root, &rem_block->link, block_comp);
	}

	block_com_set_free(&pb->common, false);
	return block_com_data(&pb->common);
}

void* heap_llrb_alloc_c(struct heap_llrb* pheap, int size) {
	void* mem = heap_llrb_alloc_try(pheap, size);

	if (mem == NULL) {
		/* we don't have enough space to alloc this size */
		int expand_size = pheap->expand_size;
		if (expand_size < 2 * size) {
			/* if the expand size is not big enough for we need */
			expand_size = 2 * size;
		}

		heap_llrb_expand_memory(pheap, expand_size);

		/* second try */
		mem = heap_llrb_alloc_try(pheap, size);
		dbg_assert(mem != NULL);
	}

	return mem;
}

void* heap_llrb_alloc_v(struct heap_llrb* pheap, int size, const char* file, int line) {
	void* buff = heap_llrb_alloc_c(pheap, size);

	if (buff == NULL)
		return NULL;

	{
		struct block_c *pbc = (struct block_c*)block_com_from_data(buff);

		block_com_debug_set_fileline(pbc, file, line);
	}

	return buff;
}

bool heap_llrb_dealloc_c(struct heap_llrb* pheap, void *buff) {
	struct block_c *pbc = block_com_from_data(buff);
	struct heap_llrb_block *pb = container_of(pbc, struct heap_llrb_block, common);
	struct block_c *prev = block_com_prev_adj(pbc);
	struct block_c *next = block_com_next_adj(pbc);

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
			/* both the prev and next block are free, merge the three */
			struct heap_llrb_block *b_prev = container_of(prev, struct heap_llrb_block, common);
			struct heap_llrb_block *b_next = container_of(next, struct heap_llrb_block, common);
			dbg_assert((void*)b_prev == (void*)prev);
			dbg_assert((void*)b_next == (void*)next);

			pheap->llrb_root = llrb_remove(pheap->llrb_root, &b_prev->link, block_comp);
			pheap->llrb_root = llrb_remove(pheap->llrb_root, &b_next->link, block_comp);

			block_com_merge(prev, next);
			pheap->llrb_root = llrb_insert(pheap->llrb_root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
		else {
			/* only the prev and curr are free, merge them */
			struct heap_llrb_block *b_prev = container_of(prev, struct heap_llrb_block, common);
			dbg_assert((void*)b_prev == (void*)prev);

			pheap->llrb_root = llrb_remove(pheap->llrb_root, &b_prev->link, block_comp);

			block_com_merge(prev, pbc);
			pheap->llrb_root = llrb_insert(pheap->llrb_root, &b_prev->link, block_comp);

			block_com_set_free(prev, true);
		}
	}
	else if (block_com_valid(next) && block_com_free(next)) {
		/* only curr and next are free, merge them */
		struct heap_llrb_block *b_next = container_of(next, struct heap_llrb_block, common);
		dbg_assert((void*)b_next == (void*)next);

		pheap->llrb_root = llrb_remove(pheap->llrb_root, &b_next->link, block_comp);

		block_com_merge(pbc, next);
		pheap->llrb_root = llrb_insert(pheap->llrb_root, &pb->link, block_comp);

		block_com_set_free(pbc, true);
	}
	else {
		/* no need to merge */
		pheap->llrb_root = llrb_insert(pheap->llrb_root, &pb->link, block_comp);

		block_com_set_free(pbc, true);
	}

	return true;
}

bool heap_llrb_dealloc_v(struct heap_llrb* pheap, void *buff, const char* file, int line) {
	bool res = heap_llrb_dealloc_c(pheap, buff);
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

/* get block info part */
void heap_llrb_get_blockinfo(void* mem_addr, /* out */ struct heap_blockinfo* info) {
	struct block_c *pbc = block_com_from_data(mem_addr);
	struct heap_llrb_block *pb = container_of(pbc, struct heap_llrb_block, common);
	struct block_c *prev = block_com_prev_adj(pbc);
	struct block_c *next = block_com_next_adj(pbc);

	if (mem_addr == NULL) {
		return;
	}

	if (!block_com_valid(pbc)) {
		/* dealloc an invalid block, break it. */
		dbg_assert(false);
	}

	/* TODO: */
	info->allocated   = !block_com_free(pbc);
	info->block_start = (void*)pbc;
	info->block_size  = block_com_size(pbc);
	info->allocable_addr  = block_com_data(pbc);
	info->allocable_size  = block_com_data_size(pbc);
	info->file        = block_com_debug_get_fileline(pbc, &info->line);

	return;
}

struct block_process_param_pack {
	pf_process_block per_block_cb;
	void*            param;
};

static void traverse_mem(struct list_link* link, void* param) {
	/* unpack the parameters */
	struct block_process_param_pack* pack = (struct block_process_param_pack*)param;
	pf_process_block per_block_cb  = pack->per_block_cb;
	void* per_block_param          = pack->param;

	/* get current memory info */
	struct block_c_pool* blk_pool  = container_of(link, struct block_c_pool, link);
	struct block_c*  pbc           = blk_pool->bc_first;
	struct block_c*  end_sent      = blk_pool->bc_end_sent;

	struct heap_blockinfo block_info;

	dbg_assert(blk_pool->bc_front_sent == blk_pool->memory);

	while (pbc != end_sent) {
		dbg_assert(block_com_valid(pbc));

		/* prepare block information */
		block_info.allocated      = !block_com_free(pbc);
		block_info.block_start    = (void*)pbc;
		block_info.block_size     = block_com_size(pbc);
		block_info.allocable_addr     = block_com_data(pbc);
		block_info.allocable_size     = block_com_data_size(pbc);
		block_info.file           = block_com_debug_get_fileline(pbc, &block_info.line);

		/* call the callback */
		per_block_cb(&block_info, per_block_param);

		/* move to next block */
		pbc = block_com_next_adj(pbc);
	}

	return;
}

/* traverse part */
void heap_llrb_walk(struct heap_llrb* pheap, pf_process_block per_block_cb, void* param) {
	struct block_process_param_pack pack;
	pack.per_block_cb = per_block_cb;
	pack.param = param;

	list_foreach_v(&pheap->memlist, traverse_mem, (void*)&pack);
}
