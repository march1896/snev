#ifndef _BLOCK_DLL_
#define _BLOCK_DLL_

/* this file defines data structure and method of using 
 * double linked list(dll) to manage free block */

#include <block_common.h>

struct heap_t;
struct block_dll_t {
	/* common part of block */
	block_c 			bc;		

	/* customize part of using double linked list as container 
	 * of block free list */
	struct block_dll_t* prev_free;
	struct block_dll_t* next_free;

	struct heap_t* 		pheap;
	const char* 		file;
	unsigned int		line;
};

block_dll* block_dllinc_find_ff(block_dll** pphead, unsigned int req) {
	assert(pphead);

	block_dll* itr = *pphead;
	while (itr != NULL) {
		itr = itr->next_free;
	}
}

block_dll* block_dllinc_find_bf(block_dll** pphead, unsigned int req) {
}

void block_dllinc_pop(block_dll** pphead, block_dll* pbd) {
}

void block_dllinc_push(block_dll** pphead, block_dll* pbd) {
}


block_dll* block_dlldec_find_ff(block_dll** pphead, unsigned int req) {
}

block_dll* block_dlldec_find_bf(block_dll** pphead, unsigned int req) {
}

void block_dlldec_pop(block_dll** pphead, block_dll* pbd) {
}

void block_dlldec_push(block_dll** pphead, block_dll* pbd) {
}


block_dll* block_dllrnd_find_ff(block_dll** pphead, unsigned int req) {
}

block_dll* block_dllrnd_find_bf(block_dll** pphead, unsigned int req) {
}

void block_dllrnd_pop(block_dll** pphead, block_dll* pbd) {
}

void block_dllrnd_push(block_dll** pphead, block_dll* pbd) {
}


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
#endif // _BLOCK_DLL_
