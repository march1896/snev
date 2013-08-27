#include <heap_pool.h>

void  heap_pool_single_init(struct heap_pool_single* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int __size) {
	h->__parent  = __parent;
	h->__alloc   = __alloc;
	h->__dealloc = __dealloc;

	/* we should save the address to next when freed */
	h->target_size = __size >= sizeof(void*) ? __size : sizeof(void*); 
	h->alloc_count = 0;
	h->level       = 0;

	h->next        = NULL;

	list_init(&h->sentinel);
}

void heap_pool_deinit(struct heap_pool* h) {
	struct list_link* itr = h->sentinel.next;

	/* return memory in the pool */
	while (itr != h->sentinel) {
		void* mem = (void*)itr;

		itr = itr->next;
		dealloc(h->__dealloc, h->__parent, mem);
	}

	/* in case someone call this function more than once */
	list_init(h->sentinel);
}

#ifndef _VERBOSE_ALLOC_DEALLOC_
void heap_pool_single_expand_memory(struct heap_pool_single* h) {
	int block_size = h->target_size;
	int expand_size = (1 << level) * block_size + sizeof(list_link);
	void* n_mem = alloc(h->__alloc, h->__parent, expand_size);
	struct list_link* link = (struct list_link*)n_mem;
	char* m_start = (char*)(link + 1);
	char* m_end   = (char*)n_mem + expand_size;
	char* m_last  = m_end - block_size;
	char* m_itr   = NULL;

	dbg_assert(m_end  == (char*)m_start + (1 << level) * block_size);
	dbg_assert(m_last == (char*)m_start + ((1 << level) - 1)* block_size);

	/* link this memory to the pool */
	list_insert_back(&h->sentinel, link);

	/* initialize the memory as a list */
	for (m_itr = m_start; m_itr != m_last; m_itr += block_size) {
		*((intptr_t*)m_itr) = (intptr_t)(m_itr + block_size);
	}

	*((intptr_t*)m_last) = (intptr_t)h->next;
	h->next = (void*)m_start;

	h->level ++;
}

void* heap_pool_single_alloc_c(struct heap_pool_single* h, int size) {
	void* mem = NULL;
	dbg_assert(size == h->size);

	if (h->next == NULL) {
		heap_pool_single_expand_memory(h);
	}
	dbg_assert(h->next != NULL);
	mem = h->next;

	h->next = (void*)(*(intptr_t)h->next);

	h->alloc_count ++;

	return mem;
}

void heap_pool_single_dealloc_c(struct heap_pool_single* h, void* buff) {
	*((intptr_t*)buff) = (intptr_t)h->next;
	h->next = buff;
	/* TODO: we could check the buff is in the buffer list */
	h->alloc_count --;
}

#else /* #defined _VERBOSE_ALLOC_DEALLOC_ */

struct pool_block_header {
	const char* file;
	int         line;
};

void heap_pool_single_expand_memory_v(struct heap_pool_single* h) {
	int block_size = sizeof(struct pool_block_header) + h->target_size;
	int expand_size = (1 << level) * block_size + sizeof(list_link);
	void* n_mem = alloc(h->__alloc, h->__parent, expand_size);
	struct list_link* link = (struct list_link*)n_mem;
	char* m_start = (char*)(link + 1);
	char* m_end   = (char*)n_mem + expand_size;
	char* m_last  = m_end - block_size;
	char* m_itr   = NULL;

	dbg_assert(m_end  == (char*)m_start + (1 << level) * block_size);
	dbg_assert(m_last == (char*)m_start + ((1 << level) - 1)* block_size);

	/* link this memory to the pool */
	list_insert_back(&h->sentinel, link);

	/* initialize the memory as a list */
	for (m_itr = m_start; m_itr != m_last; m_itr += block_size) {
		*((intptr_t*)m_itr) = (intptr_t)(m_itr + block_size);
	}

	*((intptr_t*)m_last) = (intptr_t)h->next;
	h->next = (void*)m_start;

	h->level ++;
}

void* heap_pool_single_alloc_v(struct heap_pool_single* h, int size, const char* file, int line) {
	void* mem = NULL;
	struct pool_block_header* header = NULL;
	dbg_assert(size == h->size);

	if (h->next == NULL) {
		heap_pool_single_expand_memory_v(h);
	}
	dbg_assert(h->next != NULL);

	mem = h->next;
	h->next = (void*)(*(intptr_t)h->next);

	header = (struct pool_block_header*)mem;
	header->file = file;
	header->line = line;

	h->alloc_count ++;

	return (void*)(header + 1);
}

void heap_pool_single_dealloc_v(struct heap_pool_single* h, void* buff, const char* file, int line) {
	struct pool_block_header* header = (struct pool_block_header*)buff;
	void* mem = (void*)(buff - 1);

#ifdef _HEAP_POOL_DEBUG_CHECK_
	struct list_link* link = h->sentinel.next;
	while (link != h->sentinel) {

	}
#endif

	*((intptr_t*)mem) = (intptr_t)h->next;
	h->next = mem;
	/* TODO: we could check the buff is in the buffer list */
	h->alloc_count --;
}

#endif

