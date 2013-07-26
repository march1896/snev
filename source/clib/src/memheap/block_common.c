#include <block_common.h>

inline bool block_com_valid(struct block_c* pbc) {
	if (pbc == NULL || pbc->prev_adj == NULL) {
		dbg_assert(pbc->info == 0);
		return false;
	}

	return true;
}

inline void block_com_invalidate(struct block_c* pbc) {
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

	*sent_first = buff_start;
	block_com_invalidate(*sent_first);

	*sent_last = (void*)((struct block_c*)(buff_end) - 1);
	block_com_invalidate(*sent_last);

	pb = (struct block_c*)buff_start + 1;
	block_com_init_addr(pb, *sent_first, *sent_last, 0);

	return pb;
}

inline bool block_com_free(struct block_c* pbc) {
	return (pbc->info & BLOCK_COM_EXT_MASK) != 0;
}

inline void block_com_set_free(struct block_c* pbc, bool is_free) {
	if (is_free) pbc->info |= BLOCK_COM_EXT_MASK; 
	else pbc->info &= ~BLOCK_COM_EXT_MASK;
}

inline unsigned int block_com_size(struct block_c* pbc) {
	return pbc->info & BLOCK_COM_SIZE_MASK;
}

inline unsigned int block_com_data_size(struct block_c* pbc) {
	return block_com_size(pbc) - sizeof(struct block_c);
}

inline void block_com_set_size(struct block_c* pbc, unsigned int size) {
	pbc->info = (pbc->info & BLOCK_COM_EXT_MASK) | (size & BLOCK_COM_SIZE_MASK);
}

inline struct block_c* block_com_prev_adj(struct block_c* pbc) {
	return pbc->prev_adj; 
}

inline void block_com_set_prev_adj(struct block_c* pbc, struct block_c* prev_adj) {
	pbc->prev_adj = prev_adj;
}

inline struct block_c* block_com_next_adj(struct block_c* pbc) {
	return (struct block_c*)((char*)pbc + block_com_size(pbc));
}

inline void block_com_set_next_adj(struct block_c* pbc, struct block_c* next_adj) {
	unsigned int size = (char*)next_adj - (char*)pbc;

	block_com_set_size(pbc, size);

	dbg_assert(block_com_next_adj(pbc) == next_adj);
}

inline void* block_com_data(struct block_c* pbc) {
	return (void*)((char*)pbc + sizeof(struct block_c));
}

inline struct block_c* block_com_from_data(void* addr) {
	return (struct block_c*)((char*)addr - sizeof(struct block_c));
}

inline struct block_c* block_com_split(struct block_c* pbc, unsigned int size, unsigned int thh) {
	//dbg_assert(block_com_data_size(pbc) >= size);

	char* sb_addr = (char*)block_com_data(pbc) + size;
	if (sb_addr + sizeof(struct block_c) + thh <= (char*)block_com_next_adj(pbc)) {
		/* big enough, split */
		struct block_c* sb = (struct block_c*)sb_addr;

		struct block_c* next_adj = block_com_next_adj(pbc);

		block_com_set_next_adj(pbc, sb);

		block_com_set_prev_adj(sb, pbc);
		block_com_set_next_adj(sb, next_adj);

		if (block_com_valid(next_adj))
			block_com_set_prev_adj(next_adj, sb);

		return (void*)sb;
	}

	/* not enough space to split */
	return NULL;
}

inline void block_com_merge(struct block_c* pstart, struct block_c* pend) {
	struct block_c* next_pend = block_com_next_adj(pend);

	block_com_set_next_adj(pstart, next_pend);

	if (block_com_valid(next_pend))
		block_com_set_prev_adj(next_pend, pstart);

	return;
}

inline void block_com_init_addr(struct block_c* pbc, void* prev_adj, void* next_adj , bool is_free) {
	block_com_set_prev_adj(pbc, prev_adj);
	block_com_set_next_adj(pbc, next_adj);
	block_com_set_free(pbc, is_free);
}

inline void block_com_init_size(struct block_c* pbc, void* prev_adj, unsigned int size, bool is_free) {
	block_com_set_prev_adj(pbc, prev_adj);
	block_com_set_size(pbc, size);
	block_com_set_free(pbc, is_free);
}
