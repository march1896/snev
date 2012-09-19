#ifndef _BLOCK_DLL_
#define _BLOCK_DLL_

/* this file defines data structure and method of using 
 * double linked list(dll) to manage free block */

#include <block_common.h>

struct block_dll_t {
	/* common part of block */
	struct block_c 		bc;		

	/* members to maintain double linked list */
	struct block_dll_t *prev_free;
	struct block_dll_t *next_free;
};

typedef struct block_dll_t block_dll;

/*
 * double linked list(dll) has several way to manage blocks, 
 * 1, sort free blocks in increasing order.
 * 2, sort free blocks in decreasing order.
 * 3, keep free blocks in random order.
 *
 * for all of the managing method list above, we have two ways 
 * to find a proper block that can carry enough data.
 * 1, find free block first fit.
 * 2, find free block best fit.
 *
 * from implementation point of view, below mothod should be provided.
 * 1, block_dll_find_ff (find a proper block first fit)
 * 2, block_dll_find_bf (find a proper block best fit)
 * 3, block_dll_pop (pop/erase a block from the dll)
 * 4, block_dll_push (push/insert a block into the dll)
 */

typedef enum {
	E_BDLLFT_FIRSTFIT,
	E_BDLLFT_BESTFIT,
}block_dll_find_type ;

typedef enum {
	E_BDLLST_INC,
	E_BDLLST_DEC,
	E_BDLLST_RND
}block_dll_sort_type ;

/**
 * @brief find a proper block in free list according size requirement
 *
 * @param pphead address of the header of free list
 * @param req required data size
 *
 * @return 
 */
typedef block_dll* (*pf_block_dll_find)(block_dll** pphead, unsigned int req);

/**
 * @brief pop a block from freelist, this function just change double linked list 
 *   related properties, it does not change any block_com related properties.
 *   so, after you pop the block from free list, you must manually set block_com's
 *   free property.
 *
 * @param pphead address of the header of free list
 * @param pbd the block_dll to be poped
 *
 */
typedef void       (*pf_block_dll_pop) (block_dll** pphead, block_dll* pbd);

/**
 * @brief push a block into freelist, this function just change double linked 
 *   list related properties, it does not change any block_com related properties.
 *   so, after you push a block into free list, you must manully set block_com's 
 *   free property.
 *
 * @param pphead address of the header of free list
 * @param pbd the block_dll to be pushed
 *
 */
typedef void       (*pf_block_dll_push)(block_dll** pphead, block_dll* pbd);

typedef struct block_dll_operations_h {
	pf_block_dll_find 	find;
	pf_block_dll_pop	pop;
	pf_block_dll_push   push;
} block_dll_ops;

void block_dll_make_operations(block_dll_ops* ops, block_dll_find_type ft, block_dll_sort_type st);

#endif
