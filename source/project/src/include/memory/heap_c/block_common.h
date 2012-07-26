#ifndef _BLOCK_COMMON_
#define _BLOCK_COMMON_

/********************************************************************************
 * typical heap and block memory distribution, no matter how the free block are 
 * managed, we use the following structure to construct a block pool.
 
  *****  heap memory distribution ****
                                                                                  
  		heap header	                     block buffer
      |-----------|--|-------|--------------------------------------|------|--|
    pbuff            ^       ^                                      ^      ^      
                     |   first block                                |      |      
                  sentinel                                          |   block_end    
                                                                sentinel          
                                                                                  
    the small gaps indicates align gaps                                           

*********************************************************************************
                                                                                  
  *****  block memory distribution ****
                                                                                  
        prev block               current block                        next block  
      |--------------|----------------|-----------------------------|---------|
                     ^ block header   ^      block data                           
                   pblock          data begin                                     
                                                                                  
                                                                                  
*********************************************************************************/

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
	/* prev_adj points to the previous adjacent block, DON'T use it directly, use 
	 * function block_com_prev_adj instead. */
	struct block_com_h* prev_adj; 

	/* info contains is_free and size information. */
	unsigned int 		info; 
};

typedef struct block_com_h block_c;

inline bool block_com_valid(block_c* pbc) {
	if (pbc == NULL || pbc->prev_adj == NULL) {
		assert(pbc->info == 0);
		return false;
	}

	return true;
}

inline void block_com_invalidate(block_c* pbc) {
	pbc->next_adj = NULL;
	pbc->info = 0;
}

/**
 * @brief block valid information are tested based on sentinel blocks, any heap 
 * using block_c should keep two sentinels for validation test. This function 
 * make two sentinels based on a buffer.
 *
 * @param buff_start start of the buffer
 * @param buff_end end of block buffer
 * @param sent_first (out) address of the the first sentinel
 * @param sent_last (out) address of the last sentinel
 *
 * @return firt valid block address in the buffer
 */
inline void* block_com_make_sentinels(void* buff_start, void* buff_end, void** sent_first, void** sent_last) {
	assert(buff_start && buff_end && buff_start < buff_end);

	*sent_first = buff_start;
	block_com_invalidate(*sent_first);

	*sent_last = (void*)((block_c*)(buff_end) - 1);
	block_com_invalidate(*sent_last);

	return (void*)((block_c*)buff_start + 1);
}

/**
 * @brief 
 *
 * @param pbc pointer of block_c
 *
 * @return true if pbc is in free list  
 */
inline bool block_com_free(block_c* pbc) {
	return pbc->info & BLOCK_COM_FREE_MASK;
}

/**
 * @brief 
 *
 * @param pbc pointer of block_c
 * @param is_free the new free state
 */
inline void block_com_set_free(block_c* pbc, bool is_free) {
	if (is_free) pbc->info |= BLOCK_COM_FREE_MASK; 
	else pbc->info &= ~BLOCK_COM_FREE_MASK;
}

inline unsigned int block_com_size(block_c* pbc) {
	return pbc->info & BLOCK_COM_SIZE_MASK;
}

/*
 * return maximum data size the block can carry
 */
inline unsigned int block_com_data_size(block_c* pbc) {
	return block_com_size(pbc) - sizeof(pbc);
}

inline void block_com_set_size(block_c* pbc, unsigned int size) {
	pbc->info = (pbc->info & ~BLOCK_COM_SIZE_MASK) | (size & BLOCK_COM_SIZE_MASK);
}

inline block_c* block_com_prev_adj(block_c* pbc) {
	return pbc->prev_adj; 
}

inline void block_com_set_prev_adj(block_c* pbc, block_c* prev_adj) {
	pbc->prev_adj = prev_adj;
}

inline block_c* block_com_next_adj(block_c* pbc) {
	return (block_c*)((char*)pbc + block_com_size(pbc));
}

inline void block_com_set_next_adj(block_c* pbc, block_c* next_adj) {
	unsigned int size = (char*)next_adj - (char*)pbc;

	block_com_set_size(pbc, size);
}

inline void* block_com_data(block_c* pbc) {
	return (void*)((char*)pbc + sizeof(block_c));
}

inline block_c* block_com_from_data(void* addr) {
	return (block_c*)((char*)addr - sizeof(block_c));
}

/**
 * @brief split a block into two
 *
 * @param pbc the block to split
 * @param size of data that the first block should carry after splitting
 * @param thh the minimum threadhold that the second block's data size should be,
 *        if the second block's data is less than it, this block should not be splited.
 *
 * @return address of the second block after spliting, NULL if no need to split.
 */
inline block_c* block_com_split(block_c* pbc, unsigned int size, unsigned int thh) {
	assert(block_com_data_size(pbc) >= size);

	char* sb_addr = (char*)block_com_data + size;
	if (sb_addr + sizeof(block_c) + thh <= block_com_next_adj(pbc)) {
		/* big enough, split */
		block_c* sb = (block_c*)sb_addr;

		block_c* next_adj = block_com_next_adj(pbc);

		block_com_set_next_adj(pbc, sb);

		block_com_set_prev_adj(sb, pbc);
		block_com_set_next_adj(sb, next_adj);

		if (block_com_valid(next_adj))
			block_com_set_prev_adj(next_adj, sb);
	}

	/* not enough space to split */
	return NULL;
}

/**
 * @brief the merge method is based on the assumption that the block buffer 
 *   have a start sentinel, and an end sentinel, about how to create this two
 *   sentinels. @see block_com_make_sentinels
 *
 * @param pbc the block that is to merge.
 */
inline void block_com_merge(block_c* pbc) {
	block_c* prev_adj = block_com_prev_adj(pbc);
	block_c* next_adj = block_com_next_adj(pbc);

	if (block_com_valid(prev_adj)) {
		assert(block_com_free(prev_adj));

		if (block_com_valid(next_adj)) {
			assert(block_com_free(next_adj));

			/* merge prev, cur, next */
			block_c* n_next_adj = block_com_next_adj(next_adj);
			block_com_set_prev_adj(n_next_adj, prev_adj);
			block_com_set_next_adj(prev_adj, n_next_adj);
		}
		else {
			/* merge prev, cur */

			block_com_set_prev_adj(next_adj, prev_adj);
			block_com_set_next_adj(prev_adj, next_adj);
		}
	}
	else if (block_com_valid(next_adj)) {
		assert(block_com_free(next_adj));

		/* merge cur and next */
		block_c* n_next_adj = block_com_next_adj(next_adj);
		block_com_set_next_adj(pbd, n_next_adj);
		block_com_set_prev_adj(n_next_adj, pbd);
	}
	/* no need to merge */
	return;
}

inline void block_com_init_addr(block_c* pbc, void* prev_adj, void* next_adj , bool is_free) {
	block_com_set_prev_adj(pbc, prev_adj);
	block_com_set_next_adj(pbc, next_adj);
	block_com_set_free(pbc, is_free);
}

inline void block_com_init_size(block_c* pbc, void* prev_adj, unsigned int size, bool is_free) {
	block_com_set_prev_adj(pbc, prev_adj);
	block_com_set_size(pbc, size);
	block_com_set_free(pbc, is_free);
}

#endif 
