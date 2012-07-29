#ifndef _BLOCK_EXI_
#define _BLOCK_EXI_

struct block_extra_info_t {
	/* we dont make any assumption about the heap that maintains 
	 * free list, covert to your needed type */
	void* 				owner;

	/* file and line are debug information, you can just ignore them 
	 * if you don't need them, they will not occupy space when the block
	 * is allocated */
	const char* 		file;
	unsigned int		line;
};

typedef struct block_extra_info_t block_exi;

inline void* block_exi_onwer(block_exi* pbe) {
	return pbe->owner;
}

inline void block_exi_set_owner(block_exi* pbe, void* owner) {
	pbe->owner = owner;
}

inline void block_exi_set_debuginfo(block_exi* pbe, const char* _f, unsigned int _l) {
	pbe->file = _f;
	pbe->line = _l;
	return;
}

inline void block_exi_debuginfo(block_exi* pbe, const char** pf, unsigned int* pl) {
	*pf = pbe->file;
	*pl = pbe->line;
	return;
}

#endif /* _BLOCK_EXI_ */
