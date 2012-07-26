#include <block_dll.h>

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
	assert(&(pbd->bc)== (block_c*)pbd);
	return (block_c*)pbd;
}

static void block_dll_pop_common(block_dll** pphead, block_dll* pbd) {
	assert(block_com_isfree(block_dll_to_com(pbd)));

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
		assert(prev == NULL && next == NULL);

		*pphead = pbd;
	}
	else {
		// contains at least one element
		if (prev == NULL) {
			assert(next != NULL);
			// push at the very front of freelist
			next->prev_free = pbd;
			
			*pphead = pbd;
		}
		else {
			// in the middle or at the end of the list
			assert(!next || next->prev_free == prev);

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
	assert(!block_com_isfree(block_dll_to_com(pbd)));

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
	assert(!block_com_isfree(block_dll_to_com(pbd)));

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

