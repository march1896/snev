#ifndef _HEAP_DLL_SF_
#define _HEAP_DLL_SF_

/* this file define a heap, which manages its free blocks with a single double linked list.
 * dll means double linked list, sf means single free list. */

#include <block_dll.h>

struct heap_dll_sf {
	/* point to the single free list */
	block_dll*		pfl; 

	/* total size of the heap, include heap header, block pool and all align gaps */
	unsigned int 	size;

	/* point to the start of the heap buffer. */
	void*			pbuff; 

	/* point to the first valid block, sentinel blocks are not included */
	void*			pfb;

	/* name of the heap */
	const char* 	name;

	unsigned int 	alignment;

	unsigned int 	error;
};

#endif 
