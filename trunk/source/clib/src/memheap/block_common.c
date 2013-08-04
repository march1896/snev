#include <block_common.h>

inline bool block_com_valid(struct block_c* pbc) {
	block_com_debug_check(pbc);

	if (pbc == NULL || pbc->prev_adj == NULL) {
		dbg_assert(pbc->info == 0);
		return false;
	}

	return true;
}

inline void block_com_invalidate(struct block_c* pbc) {
	block_com_debug_check(pbc);

	pbc->prev_adj = NULL;
	pbc->info = 0;
}

inline struct block_c* block_com_make_sentinels(
		void* buff_start, 
		void* buff_end, 
		struct block_c **sent_first, 
		struct block_c **sent_last) 
{
	struct block_c *pb;
	dbg_assert(buff_start && buff_end && buff_start < buff_end);

	*sent_first = (struct block_c*)buff_start;
	block_com_debug_init(*sent_first);
	block_com_invalidate(*sent_first);

	*sent_last = ((struct block_c*)(buff_end) - 1);
	block_com_debug_init(*sent_last);
	block_com_invalidate(*sent_last);

	pb = (struct block_c*)buff_start + 1;
	block_com_debug_init(pb);
	block_com_init_addr(pb, *sent_first, *sent_last, 0);	

	return pb;
}

inline bool block_com_free(struct block_c* pbc) {
	block_com_debug_check(pbc);

	return (pbc->info & BLOCK_COM_EXT_MASK) != 0;
}

inline void block_com_set_free(struct block_c* pbc, bool is_free) {
	block_com_debug_check(pbc);

	if (is_free) pbc->info |= BLOCK_COM_EXT_MASK; 
	else pbc->info &= ~BLOCK_COM_EXT_MASK;
}

inline unsigned int block_com_size(struct block_c* pbc) {
	block_com_debug_check(pbc);

	return pbc->info & BLOCK_COM_SIZE_MASK;
}

inline unsigned int block_com_data_size(struct block_c* pbc) {
	block_com_debug_check(pbc);

	return block_com_size(pbc) - sizeof(struct block_c);
}

inline void block_com_set_size(struct block_c* pbc, unsigned int size) {
	block_com_debug_check(pbc);

	pbc->info = (pbc->info & BLOCK_COM_EXT_MASK) | (size & BLOCK_COM_SIZE_MASK);
}

inline struct block_c* block_com_prev_adj(struct block_c* pbc) {
	block_com_debug_check(pbc);

	return pbc->prev_adj; 
}

inline void block_com_set_prev_adj(struct block_c* pbc, struct block_c* prev_adj) {
	block_com_debug_check(pbc);

	pbc->prev_adj = prev_adj;
}

inline struct block_c* block_com_next_adj(struct block_c* pbc) {
	block_com_debug_check(pbc);

	return (struct block_c*)((char*)pbc + block_com_size(pbc));
}

inline void block_com_set_next_adj(struct block_c* pbc, struct block_c* next_adj) {
	unsigned int size = (char*)next_adj - (char*)pbc;

	block_com_debug_check(pbc);

	block_com_set_size(pbc, size);

	dbg_assert(block_com_next_adj(pbc) == next_adj);
}

inline void* block_com_data(struct block_c* pbc) {
	block_com_debug_check(pbc);

	return (void*)((char*)pbc + sizeof(struct block_c));
}

inline struct block_c* block_com_from_data(void* addr) {
	struct block_c* pbc = (struct block_c*)((char*)addr - sizeof(struct block_c));

	block_com_debug_check(pbc);

	return pbc;
}

inline struct block_c* block_com_split(struct block_c* pbc, unsigned int size, unsigned int thh) {
	//dbg_assert(block_com_data_size(pbc) >= size);

	char* sb_addr = (char*)block_com_data(pbc) + size;

	block_com_debug_check(pbc);

	if (sb_addr + sizeof(struct block_c) + thh <= (char*)block_com_next_adj(pbc)) {
		/* big enough, split */
		struct block_c* sb = (struct block_c*)sb_addr;
		struct block_c* next_adj = block_com_next_adj(pbc);

		block_com_debug_init(sb);

		block_com_set_next_adj(pbc, sb);

		block_com_set_prev_adj(sb, pbc);
		block_com_set_next_adj(sb, next_adj);

		if (block_com_valid(next_adj))
			block_com_set_prev_adj(next_adj, sb);

		return sb;
	}

	/* not enough space to split */
	return NULL;
}

inline void block_com_merge(struct block_c* pstart, struct block_c* pend) {
	struct block_c* next_pend = block_com_next_adj(pend);

	block_com_debug_check(pstart);
	block_com_debug_check(pend);

	block_com_set_next_adj(pstart, next_pend);

	if (block_com_valid(next_pend))
		block_com_set_prev_adj(next_pend, pstart);

	/* maybe we should deinit all debug information for the blocks between 
	 * start and end */
	block_com_debug_deinit(pend);

	return;
}

inline void block_com_init_addr(struct block_c* pbc, void* prev_adj, void* next_adj , bool is_free) {
	block_com_debug_check(pbc);
	block_com_debug_check(prev_adj);
	block_com_debug_check(next_adj);

	block_com_set_prev_adj(pbc, (struct block_c*)prev_adj);
	block_com_set_next_adj(pbc, (struct block_c*)next_adj);
	block_com_set_free(pbc, is_free);
}

inline void block_com_init_size(struct block_c* pbc, void* prev_adj, unsigned int size, bool is_free) {
	block_com_debug_check(pbc);
	block_com_debug_check(prev_adj);

	block_com_set_prev_adj(pbc, (struct block_c*)prev_adj);
	block_com_set_size(pbc, size);
	block_com_set_free(pbc, is_free);
}

#ifdef _MEM_DEBUG_
#define BLOCK_C_DEBUG_HEADER 0X03F09CC3
#define BLOCK_C_DEBUG_FOOTER 0X5714959E
inline void block_c_debug_check(struct block_c_debug* pbc) {
	dbg_assert(
		pbc->header == BLOCK_C_DEBUG_HEADER &&
		pbc->footer == BLOCK_C_DEBUG_FOOTER
		);
}
inline void block_c_debug_init(struct block_c_debug* pbc) {
	pbc->header = BLOCK_C_DEBUG_HEADER;
	pbc->footer = BLOCK_C_DEBUG_FOOTER;
}
inline void block_c_debug_deinit(struct block_c_debug* pbc) {
	/*
	TODO:
	we can not deinit it here, because we may check a delete memory block to make sure it 
	is not delete multiple times 
	pbc->header = 0;
	pbc->footer = 0;
	*/
}

inline void block_c_debug_set_fileline(struct block_c_debug* pbc, const char* file, int line) {
	pbc->file = file;
	pbc->line = line;
}
inline const char* block_c_debug_get_fileline(struct block_c_debug* pbc, int* line) {
	*line = pbc->line;
	return pbc->file;
}

#else

static char* no_file_info = "no file info! please use _MEM_DEBUG_ to compile.\n";
inline const char* block_c_clean_get_fileline(struct block_c_clean* pbc, int* line) {
	*line = 0;
	return no_file_info;
}

#endif
