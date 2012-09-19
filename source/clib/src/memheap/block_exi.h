#ifndef _block_ext_
#define _block_ext_

struct block_ext {
	/* we dont make any assumption about the heap that maintains 
	 * free list, covert to your needed type */
	void* 				owner;

	/* file and line are debug information, you can just ignore them 
	 * if you don't need them, they will not occupy space when the block
	 * is allocated */
	const char* 		file;
	unsigned int		line;
};

#endif /* _block_ext_ */
