#ifndef _BLOCK_COMMON_
#define _BLOCK_COMMON_

#include <heap_def.h>

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
 *       it should know the prevIous/next block address to do the merge work).
 *
 * when the block is not allocated, it is in the free list, we should know:
 *    1, the size of the block. (in order to decide if it's allocatable by a given size)
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
 *       is not useful for in_freelist blocks. Indeed, when maintaining the free list,
 *       prev_adj is not need to known. But when we allocate a free block from free list,
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
 * information that both allocated/in_free_list blocks need. every custom block header
 * should contains this struct at the very beginning of it*/

#define BLOCK_COM_EXT_BIT 31

#define BLOCK_COM_EXT_MASK (1 << BLOCK_COM_EXT_BIT)

#define BLOCK_COM_SIZE_MASK (~BLOCK_COM_EXT_MASK)

/* 
 * struct block_c is the minimum length of block header for allocated blocks
 *
 * a concrete memory management algorithm should maintain its own links to manage
 * memory into a 'free list'.
 */

struct block_c_clean {
	struct block_c_clean* prev_adj; 
	unsigned int          info; 
};

struct block_c_debug {
	unsigned int          header;

	struct block_c_debug* prev_adj;
	unsigned int          info;

	const char*           file;
	unsigned              line;

	unsigned int          footer;
};

#ifdef _MEM_DEBUG_
extern inline void block_c_debug_check(struct block_c_debug* pbc);
extern inline void block_c_debug_init(struct block_c_debug* pbc);
extern inline void block_c_debug_deinit(struct block_c_debug* pbc);

extern inline void block_c_debug_set_fileline(struct block_c_debug* pbc, const char* file, int line);
extern inline const char* block_c_debug_get_fileline(struct block_c_debug* pbc, int* line);

#define block_c block_c_debug

/* by using these macros, control the efficiency explicitly. */
#define block_com_debug_check(x)  block_c_debug_check(x)
#define block_com_debug_init(x)   block_c_debug_init(x)
#define block_com_debug_deinit(x) block_c_debug_deinit(x)
#define block_com_debug_set_fileline(c, f, l) block_c_debug_set_fileline(c, f, l)
#define block_com_debug_get_fileline(c, e);   block_c_debug_set_fileline(c, e)

#else
extern inline const char* block_c_clean_get_fileline(struct block_c_clean* pbc, int* line);
inline void block_c_clean_donothing(void* x) {}
inline void block_c_clean_donothingv(void* x, void* y, void* z) {}

/* by using these macros, control the efficiency explicitly. */
#define block_c block_c_clean

#define block_com_debug_check(x) block_c_clean_donothing(x)
#define block_com_debug_init(x)  block_c_clean_donothing(x)
#define block_com_debug_deinit(x) block_c_clean_donothing(x)
#define block_com_debug_set_fileline(c, f, l) block_c_clean_donothingv(x)
#define block_com_debug_get_fileline(c, e);   block_c_clean_set_fileline(c, e)

#endif
/* 
 * Check if a block is valid block, valid means it's in the allocated list
 * or in the free list, not include the sentinel blocks.
 */
extern inline bool block_com_valid(struct block_c* pbc);

/*
 * Make a block invalide, typically for sentinel blocks.
 */
extern inline void block_com_invalidate(struct block_c* pbc);

/**
 * @brief get the extra alpha bit information
 *
 * @param pbc pointer of struct block_c
 *
 * @return true if extra alpha is set
 */
extern inline bool block_com_free(struct block_c* pbc);

/**
 * @brief set the extra alpha bit
 *
 * @param pbc pointer of struct block_c
 * @param is_free the new free state
 */
extern inline void block_com_set_free(struct block_c* pbc, bool is_free);

/**
 * @brief get the total block size, include block header
 *
 * @param pbc pointer of the block
 *
 * @return the block size
 */
extern inline unsigned int block_com_size(struct block_c* pbc);

/**
 * @brief get the maximum data size that the block can carry.
 *
 * @param pbc pointer of the block
 *
 * @return maximum data size
 */
extern inline unsigned int block_com_data_size(struct block_c* pbc);

extern inline void block_com_set_size(struct block_c* pbc, unsigned int size);

extern inline struct block_c* block_com_prev_adj(struct block_c* pbc);

extern inline void block_com_set_prev_adj(struct block_c* pbc, struct block_c* prev_adj);

extern inline struct block_c* block_com_next_adj(struct block_c* pbc);

extern inline void block_com_set_next_adj(struct block_c* pbc, struct block_c* next_adj);

extern inline void* block_com_data(struct block_c* pbc);

extern inline struct block_c* block_com_from_data(void* addr);

extern inline void block_com_init_addr(struct block_c* pbc, void* prev_adj, void* next_adj , bool ext);

extern inline void block_com_init_size(struct block_c* pbc, void* prev_adj, unsigned int size, bool ext);

/**
 * @brief block valid information are tested based on sentinel blocks, any heap 
 * using struct block_c should keep two sentinels for validation test. This function 
 * make two sentinels based on a buffer.
 *
 * @param buff_start start of the buffer
 * @param buff_end end of block buffer
 * @param sent_first (out) address of the the first sentinel
 * @param sent_last (out) address of the last sentinel
 *
 * @return firt valid block address in the buffer
 */
extern inline struct block_c* block_com_make_sentinels(void* buff_start, void* buff_end, struct block_c **sent_first, struct block_c **sent_last);

/**
 * @brief split a block into two
 *
 * @param pbc the block to split
 * @param size of data that the first block should carry after splitting
 * @param thh the minimum threadhold that the second block's data size should be,
 *        if the second block's data is less than it, this block should not be splited.
 *
 * @return address of the second block after splitting, NULL if no need to split.
 */
extern inline struct block_c* block_com_split(struct block_c* pbc, unsigned int size, unsigned int thh);

/**
 * @brief merge a list of block_com into one block, after merging, 
 *   the start block will be the merged block.
 *
 * @param pstart start of the blocks to merge 
 * @param pend end of blocks to merge
 */
extern inline void block_com_merge(struct block_c* pstart, struct block_c* pend);

#endif 
