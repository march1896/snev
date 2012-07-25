#ifndef _BLOCK_DLL_
#define _BLOCK_DLL_

/* this file defines data structure and method of using 
 * double linked list(dll) to manage free block */

#include <block_common.h>

struct block_dll_t {
	/* common part of block */
	block_c 			bc;		

	/* customize part of using double linked list as container 
	 * of block free list */
	struct block_dll_t* prev_free;
	struct block_dll_t* next_free;

	/* we dont make any assumption about the heap that maintains 
	 * free list, covert to your needed type */
	void* 				heap;

	/* file and line are debug information, you can just ignore them 
	 * if you don't need them, they will not occupy space when the block
	 * is allocated */
	const char* 		file;
	unsigned int		line;
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

block_dll* block_dllinc_find_ff (block_dll** pphead, unsigned int req);
block_dll* block_dllinc_find_bf (block_dll** pphead, unsigned int req);
void       block_dllinc_pop     (block_dll** pphead, block_dll* pbd);
void       block_dllinc_push    (block_dll** pphead, block_dll* pbd);

block_dll* block_dlldec_find_ff (block_dll** pphead, unsigned int req);
block_dll* block_dlldec_find_bf (block_dll** pphead, unsigned int req);
void       block_dlldec_pop     (block_dll** pphead, block_dll* pbd);
void       block_dlldec_push    (block_dll** pphead, block_dll* pbd);

block_dll* block_dllrnd_find_ff (block_dll** pphead, unsigned int req);
block_dll* block_dllrnd_find_bf (block_dll** pphead, unsigned int req);
void       block_dllrnd_pop     (block_dll** pphead, block_dll* pbd);
void       block_dllrnd_push    (block_dll** pphead, block_dll* pbd);


/* block_dll additional information related operations */
inline void block_dll_set_heap(block_dll* pbd, void* ph) {
	pbd->heap = h;
}

/* 
 * if you did not set the heap info, do not get heap info, 
 * here we DO NOT guarantee the return value if it's not set
 */
inline void* block_dll_get_heap(block_dll* pbd) {
	return pbd->heap;
}

/* set debug information to block */
inline void block_dll_set_debuginfo(block_dll* pbd, const char* _f, unsigned int _l) {
	pbd->file = _f;
	pbd->line = _l;
	return;
}

/* get debug information from block */
inline void block_dll_get_debuginfo(block_dll* pbd, const char** pf, unsigned int* pl) {
	*pf = pbd->file;
	*pl = pbd->line;
	return;
}
#endif // _BLOCK_DLL_
