#include <heap_pool.h>

void  heap_pool_single_init(struct heap_pool_single* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int __size) {
	h->__parent  = __parent;
	h->__alloc   = __alloc;
	h->__dealloc = __dealloc;

	/* we should save the address to next when freed */
	h->psize      = __size >= sizeof(void*) ? __size : sizeof(void*); 
	h->count     = 0;
	h->level     = 0;

	h->next      = NULL;

	list_init(&h->sentinel);
}

void heap_pool_single_expand_memory(struct heap_pool_single* h) {
	int expand_size = (1 << level) * h->psize + sizeof(list_link);
	void* n_mem = alloc(h->__alloc, h->__parent, );
	struct list_link* link = (struct list_link*)n_mem;
	char* m_start = (n_mem + 1);
	char* m_end   = (char*)n_mem + expand_size;
	char* m_last  = m_end - h->psize;

	dbg_assert(m_end  == (char*)m_start + (1 << level) * h->psize);
	dbg_assert(m_last == (char*)m_start + ((1 << level) - 1)* h->psize);

	/* link this memory to the pool */
	list_insert_back(&h->sentinel, link);

	/* initialize the memory as a list */
	for (; m_start != m_last; m_start += h->psize) {
		*((intptr_t*)m_start) = (intptr_t)(m_start + h->psize);
	}

	*((intptr_t*)m_last) = (intptr_t)h->next;
	h->next = (void*)m_start;
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
}

void heap_pool_single_dealloc_c(struct heap_pool_single* h, void* buff) {
	*((intptr_t*)buff) = (intptr_t)h->next;
	h->next = buff;
	/* TODO: we could check the buff is in the buffer list */
}

void heap_pool_deinit(struct heap_pool* h) {
	/* should we free all memory in the allocated list? */
	/* TODO: */
	//list_foreach_v();
}

#ifndef _VERBOSE_ALLOC_DEALLOC_
void* heap_pool_alloc_c(struct heap_pool* h, int size) {
	for (i = 0; i < h->used_pools; i ++) {
	}
	int __size = size + sizeof(struct list_link);
	void* buff = alloc(h->__alloc, h->__parent, size);

	dbg_assert(buff != NULL);
	/* link the allocated block in to list */
	{
		struct list_link* n_link = (struct list_link*)buff;
		list_insert_back(&h->allocated, n_link);
	}

	/* return the 'real' address */
	return (char*)buff + sizeof(struct list_link);
}

bool  heap_pool_dealloc_c(struct heap_pool* h, void* buff) {
	/* first get the real address of the block */
	void* real_addr = (char*)buff - sizeof(struct list_link);
	struct list_link* link = (struct list_link*)real_addr;

	/* remove it from the allocated list */
	list_remove(&h->allocated, link);

	/* return the memory to the 'real' heap */
	return h->__dealloc(h->__parent, real_addr);
	/* using the raw __dealloc to keep the client file & line */
	/*dealloc(h->__dealloc, h->__parent);*/
}

#else /* #defined _VERBOSE_ALLOC_DEALLOC_ */

void* heap_pool_alloc_v(struct heap_pool* h, int size, const char* file ,int line) {
	int __size = size + sizeof(struct list_link);
	//void* buff = alloc(h->__alloc, h->__parent, __size);
	void* buff = h->__alloc(h->__parent, __size, file, line);

	dbg_assert(buff != NULL);
	/* link the allocated block in to list */
	{
		struct list_link* n_link = (struct list_link*)buff;
		list_insert_back(&h->allocated, n_link);
	}

	/* return the 'real' address */
	buff = (void*)((char*)buff + sizeof(struct list_link));
	return buff;
}


bool  heap_pool_dealloc_v(struct heap_pool* h, void* buff, const char* file, int line) {
	/* first get the real address of the block */
	void* real_addr = (char*)buff - sizeof(struct list_link);
	struct list_link* link = (struct list_link*)real_addr;

	/* remove it from the allocated list */
	list_remove(&h->allocated, link);

	/* using the raw __dealloc to keep the client file & line */
	/*dealloc(h->__dealloc, h->__parent, real_addr, file, line);*/
	return h->__dealloc(h->__parent, real_addr, file, line);
}

#endif

