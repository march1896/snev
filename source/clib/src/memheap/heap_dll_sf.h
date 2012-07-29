#ifndef _HEAP_DLL_SF_
#define _HEAP_DLL_SF_

/* this file define a heap, which manages its free blocks with a single double linked list.
 * dll means double linked list, sf means single free list. */

#include <block_dll.h>

#include <heap_interface.h>

struct heap_dll_sf_t {
	/* point to the single free list */
	block_dll*		pfreelist; 

	/* point to the start of the heap buffer. */
	void*			pbuff; 

	/* total size of the heap, include heap header, block pool and all align gaps */
	unsigned int 	size;

	/* point to the first valid block, sentinel blocks are not included */
	void*			pfirstblock;

	unsigned int 	flag;

	unsigned int 	error;

	block_dll_ops   dllops;
};

typedef struct heap_dll_sf_t heap_dsf;

heap_handle heap_dsf_init(void *buff, int size);

void  heap_dsf_destroy(heap_handle pheap);

void* heap_dsf_alloc  (heap_handle pheap, int size, const char* file, size_t line);

void  heap_dsf_dealloc(heap_handle pheap, void *buff, const char* file, size_t line);

void* heap_dsf_realloc(heap_handle pheap, void *buff, int size, const char* file, size_t line);

void  heap_dsf_dump   (heap_handle pheap);

/* align function, alignment must be power of 2.
 * this may be move to some common place elsewhere */
static inline unsigned int allign_down(unsigned int x, unsigned int alignment) {
	return x & (~(alignment - 1));
}

static inline unsigned int allign_up(unsigned int x, unsigned int alignment) {
	return (x + alignment - 1) & (~(alignment - 1));
}

heap_handle heap_dsf_init(void *buff, int size) {
	assert(size > sizeof(heap_dsf) + 50 * sizeof(block_dll));

	heap_dsf* ph = (heap_dsf*)buff;
	ph->pbuff = buff;
	ph->size = size;
	ph->flag = 0;
	ph->error = 0;
	ph->pfreelist = NULL;
	ph->pfirstblock = NULL;
	block_dll_make_operations(ph->dllops, E_BDLLFT_FIRSTFIT, E_BDLLST_INC);

	void* bbs = (char*)buff + sizeof(heap_dsf); /* block buffer start */
	void* bbe = (char*)buff + size; /* block buffer end */

	void* fs = NULL;  /* first sentinel */
	void* ls = NULL;  /* last sentinel */

	ph->pfirstblock = block_com_make_sentinels(bbs, bbe, &fs, &ls);

	block_com_init_addr((block_c*)ph->pfirstblock, fs, ls, true);

	(ph->dllops).push(&ph->pfreelist, (block_dll*)ph->pfirstblock);

	return (heap_handle)ph;
}

void  heap_dsf_destroy(heap_handle pheap) {
}

void* heap_dsf_alloc(heap_handle pheap, int size, const char* file, size_t line) {
	heap_dsf* ph = (heap_dsf*) pheap;

	block_dll* pbd = (ph->dllops).find((block_dll*)(&ph->pfreelist), size);

	/* no free block found, return NULL,
	 * maybe should raise an error */
	if (pbd == NULL) return NULL;

	/* default split threadhold, may be should keep in heap_dsf */
	block_c* pspt = block_com_split((block_c*)pbd, size, 64);

	/* pop current block from freelist */
	(ph->dllops).pop(&ph->pfreelist, pbd);
	block_com_set_free(pspt, true);

	/* push the splited block into freelist */
	if (pspt) {
		(ph->dllops).push(&ph->pfreelist, (block_dll*)pspt);
		block_com_set_free((block_c*)pbd, false);
	}

	return block_com_data((block_c*)pbd);
}

void  heap_dsf_dealloc(heap_handle pheap, void *buff, const char* file, size_t line) {
	heap_dsf* ph = (heap_dsf*)pheap;

	block_c* pbc = block_com_from_data(buff);

	block_c* prev_adj = block_com_prev_adj(pbc);
	block_c* next_adj = block_com_next_adj(pbc);

	if (block_com_valid(prev_adj) && block_com_free(prev_adj)) {
		(pb->dllops).pop(&ph->freelist, prev_adj);
	}
}

void* heap_dsf_realloc(heap_handle pheap, void *buff, int size, const char* file, size_t line);

void  heap_dsf_dump   (heap_handle pheap);
#endif 
