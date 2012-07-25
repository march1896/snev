#ifndef _BLOCK_COMMON_
#define _BLOCK_COMMON_

/* Heap block definitions */
/* design requirements for struct block:
 *
 * when the block is allocated, it is not in the free list, we should know:
 *    1, the previous/next adjacent block. (when it is given back to the heap
 *       it should know the preivous/next block address to do the merge work).
 *
 * when the block is not allocated, it is in the free list, we should know:
 *    1, the size of the block. (in order to deside if it's allocatable)
 *    2, the previous/next free block. (to maintain the free list)
 *    3, the next adjacent block,(it is a little hard to understand this 
 *       rule, consider the current block is in free list, and the prev/next 
 *       adjacent blocks are both allocated, and now the prev adjacent freed and 
 *       combined with the current block, now the next block is still allocated,
 *       so it should know the previous adjacent block, which will be changed 
 *       after the combination, so we should notify the next adjacent block that
 *       its prev is changed, the prev adjacent block is no need to know since its 
 *       next pointer will never changed if it's allocated), this rule is duplicated
 *       with rule 1 since if we know the address and size of a block, we will 
 *       certainly know the next adjacent block.
 *    4, the previous adjacent block( this is the hardest part to understand, I
 *       was unsuccessfully design the data structure, because I thought this member
 *       is not useful for in_freelist blocks. Indeed, when maintianing the free list,
 *       prev_adj is not need to known. But when we allocate a free block from freelist,
 *       the block comes to allocated immediately, in other words, we must know the 
 *       prev_adj/next_adj immediately. I spent a lot of time to define data structure 
 *       and algorithm based on the absent of prev_adj for in_freelist block, until 
 *       I am starting to write allocate(), what a pity.
 *
 *  at any time, when a get a pointer of a block, we could tell if the block is in
 *    free list. (this is also related for merging work, when we try to merge 
 *    the current block with prev/next adjacent block, we should first prev/next 
 *    adjacent blocks are in free list)
 *  at any time, we could tell if the block is valid, because we may get a pointer of
 *    a block from the adjacent block. 
 * 	
 * 	conclusion:
 * 	  Ideally, for allocated block, we should keep the header information as few as 
 * 	  possible, at least we could get the prev/next adjacent block, and we could tell 
 * 	  if the block is valid or free.
 * 	  from implementation point of view, if we use two pointers to keep prev/next adjacent
 * 	  blocks, we should add another member to suggest if the block is free.
 * 	  If we use a pointer points to prev adjacent block, and record the size of current
 * 	  block, the next adjacent block could be computed from the current address and 
 * 	  the size of the block, then we could compress is_free information into size, of 
 * 	  course, the maximum presented size will decrease.
 * 	  for is_valid, we could put two sentinel in front/back end of the block buffer.
 *
 *    For in_freelist block, we are not care about the header length, if it is not 
 *    toooooooooo long.
 */

/* This file defines the common part of block header, it keeps the minimum common 
 * information that both allocated/infreelist blocks need. every custom block header
 * should contains this struct at the very beginning of it*/


#define BLOCK_COM_FREE_BIT 31
#define BLOCK_COM_FREE_MASK (1 << BLOCK_COM_FREE_BIT)
#define BLOCK_COM_SIZE_MASK (BLOCK_COM_FREE_MASK - 1)

struct block_com_h {
	// prev_adj points to the previous adjacent block, DON'T use it directly, use 
	// function block_com_prev_adj instead.
	struct block_com_h* prev_adj; 

	// info contains is_free and size information.
	unsigned int 		info; 
};

typedef struct block_com_h block_c;

// return true if pbc is valid
inline bool block_com_is_sentinel(block_c* pbc) {
	if (pbc == NULL || pbc->prev_adj == NULL) {
		assert(pbc->info == 0);
		return false;
	}

	return true;
}

// mark pbc as an invalid block
inline void block_com_make_sentinel(block_c* pbc) {
	pbc->next_adj = NULL;
	pbc->info = 0;
}

/* block valid information are tested based on sentinel blocks, any heap using block_c
 * should keep two sentinels for validation test. 
 * This function make two sentinels based on the given block buffer(from *ppstart to *ppend),
 * when sentinels are prepared, ppstart and ppend will points to the new valid block 
 * start/end buffer.*/
inline void block_com_make_sentinels(void** ppstart, void** ppend) {
	assert(ppstart && ppend && (*ppstart < *ppend));

	block_c* sentinel = (block_c*)*ppstart;
	block_com_make_sentinel(sentinel);
	ppstart = (void*)&(sentinel + 1);

	sentinel = (block_c*)((char*)(*ppend) - sizeof(block_c));
	block_com_make_sentinel(sentinel);
	ppend = (void*)&sentinel;

	return;
}

/* 
 * return true if pbc is in free list 
 */
inline bool block_com_isfree(block_c* pbc) {
	return pbc->info & BLOCK_COM_FREE_MASK;
}

/*
 * mark pbc as it is in free list
 */
inline void block_com_markfree(block_c* pbc) {
	pbc->info |= BLOCK_COM_FREE_MASK; 
}

/*
 * return size of pbc
 */
inline unsigned int block_com_size(block_c* pbc) {
	return pbc->info & BLOCK_COM_SIZE_MASK;
}

/*
 * return maximum data size the block can carry
 */
inline unsigned int block_com_data_size(block_c* pbc) {
	return block_com_size(pbc) - sizeof(pbc);
}

/*
 * get the previous adjacent block.
 * return NULL if no previous adjacent block available.
 */
inline block_c* block_com_prev_adj(block_c* pbc) {
	return block_com_is_sentinel(pbc->prev_adj) ? pbc->prev_adj : NULL;
}

/*
 * get the next adjacent block.
 * return NULL if no next adjacent block available.
 */
inline block_c* block_com_next_adj(block_c* pbc) {
	block_c* next = (block_c*)((char*)pbc + block_com_size(pbc));

	return block_com_is_sentinel(next) ? next : NULL;
}

/* 
 * return the data address of pbc
 */
inline void* block_com_get_data(block_c* pbc) {
	return (void*)((char*)pbc + sizeof(block_c));
}

/* 
 * return the block address according to data its carrying
 */
inline block_c* block_com_from_data(void* addr) {
	return (block_c*)((char*)addr - sizeof(block_c));
}

#endif //_BLOCK_COMMON_
