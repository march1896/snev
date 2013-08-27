#include "heap_pool.h"

void  heap_spool_init(struct heap_spool* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int __size) {
	h->__parent  = __parent;
	h->__alloc   = __alloc;
	h->__dealloc = __dealloc;

	/* we should save the address to next when freed */
	h->target_size = __size >= sizeof(void*) ? __size : sizeof(void*); 
	h->alloc_count = 0;
	h->start_level = HEAP_POOL_START_LEVEL;
	h->level       = HEAP_POOL_START_LEVEL;

	h->next        = NULL;

	list_init(&h->sentinel);
}

void heap_spool_deinit(struct heap_spool* h) {
	struct list_link* itr = h->sentinel.next;

	/* return memory in the pool */
	while (itr != &h->sentinel) {
		void* mem = (void*)itr;

		itr = itr->next;
		dealloc(h->__dealloc, h->__parent, mem);
	}

	/* in case someone call this function more than once */
	list_init(&h->sentinel);
}

/* this is only useful for verbose block */
struct pool_block_header {
	const char* file;
	int         line;
};

void heap_spool_expand_memory(struct heap_spool* h) {
#ifdef _MEM_DEBUG_
	int block_size = sizeof(struct pool_block_header) + h->target_size;
#else 
	int block_size = h->target_size;
#endif
	int expand_size = (1 << h->level) * block_size + sizeof(struct list_link);
	void* n_mem = alloc(h->__alloc, h->__parent, expand_size);
	struct list_link* link = (struct list_link*)n_mem;
	char* m_start = (char*)(link + 1);
	char* m_end   = (char*)n_mem + expand_size;
	char* m_last  = m_end - block_size;
	char* m_itr   = NULL;

	dbg_assert(m_end  == (char*)m_start + (1 << h->level) * block_size);
	dbg_assert(m_last == (char*)m_start + ((1 << h->level) - 1)* block_size);

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

#ifndef _VERBOSE_ALLOC_DEALLOC_

void* heap_spool_alloc_c(struct heap_spool* h, int size) {
	void* mem = NULL;
	dbg_assert(size == h->target_size);

	if (h->next == NULL) {
		heap_spool_expand_memory(h);
	}
	dbg_assert(h->next != NULL);
	mem = h->next;

	h->next = (void*)(*(intptr_t*)h->next);

	h->alloc_count ++;

	return mem;
}

bool heap_spool_dealloc_c(struct heap_spool* h, void* buff) {
	*((intptr_t*)buff) = (intptr_t)h->next;
	h->next = buff;
	
#ifdef _HEAP_POOL_DEBUG_CHECK_
	{
		struct list_link* link = h->sentinel.next;
		int level = h->start_level;
		while (link != &h->sentinel) {
			int block_size = h->target_size;
			int mem_size = (1 << h->level) * block_size + sizeof(struct list_link);
			char* m_start = (char*)(link + 1);
			char* m_last  = (char*)link + mem_size - sizeof(struct list_link);

			if ((char*)buff >= m_start && (char*)buff <= m_last) {
				if (((char*)buff - m_start) % block_size == 0) {
					break;
				}
			}
			level ++;
		}
		dbg_assert(link != &h->sentinel);
	}
#endif

	h->alloc_count --;

	return true;
}

#else /* #defined _VERBOSE_ALLOC_DEALLOC_ */

void* heap_spool_alloc_v(struct heap_spool* h, int size, const char* file, int line) {
	void* mem = NULL;
	struct pool_block_header* header = NULL;
	dbg_assert(size == h->target_size);

	if (h->next == NULL) {
		heap_spool_expand_memory(h);
	}
	dbg_assert(h->next != NULL);

	mem = h->next;
	h->next = (void*)(*(intptr_t*)h->next);

	header = (struct pool_block_header*)mem;
	header->file = file;
	header->line = line;

	h->alloc_count ++;

	return (void*)(header + 1);
}

bool heap_spool_dealloc_v(struct heap_spool* h, void* buff, const char* file, int line) {
	struct pool_block_header* header = (struct pool_block_header*)buff;
	void* mem = (void*)(header - 1);

#ifdef _HEAP_POOL_DEBUG_CHECK_
	{
		struct list_link* link = h->sentinel.next;
		int level = h->start_level;
		while (link != &h->sentinel) {
			int block_size = h->target_size;
			int mem_size = (1 << h->level) * block_size + sizeof(struct list_link);
			char* m_start = (char*)(link + 1);
			char* m_last  = (char*)link + mem_size - sizeof(struct list_link);

			if ((char*)buff >= m_start && (char*)buff <= m_last) {
				if (((char*)buff - m_start) % block_size == 0) {
					break;
				}
			}
			level ++;
		}
		dbg_assert(link != &h->sentinel);
	}
#endif

	*((intptr_t*)mem) = (intptr_t)h->next;
	h->next = mem;
	/* TODO: we could check the buff is in the buffer list */
	h->alloc_count --;

	return true;
}

#endif

void heap_mpool_init(struct heap_mpool* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int max_diff_type) {
	h->__parent  = __parent;
	h->__alloc   = __alloc;
	h->__dealloc = __dealloc;

	h->num_pools = max_diff_type;
	h->used_pools = 0;

	h->spools = (struct heap_spool**)
		alloc(h->__alloc, h->__parent, sizeof(struct heap_spool*) * h->num_pools);

	h->alloc_count = 0;
}

void heap_mpool_deinit(struct heap_mpool* h) {
	int i = 0;
	for (i = 0; i < h->used_pools; i ++) {
		dbg_assert(h->spools[i] != NULL);
		heap_spool_deinit(h->spools[i]);

		dealloc(h->__dealloc, h->__parent, h->spools[i]);
	}
	dealloc(h->__dealloc, h->__parent, h->spools);
}

#ifndef _VERBOSE_ALLOC_DEALLOC_
void* heap_mpool_alloc_c(struct heap_mpool* h, int __size) {
	void* mem;
	int i = 0;
	/* for mpool we must know the size of the buff to decide which spool to dealloc */
	int size = __size + sizeof(int);

	for (i = 0; i < h->used_pools; i ++) {
		if (h->spools[i]->target_size == size) {
			break;
		}
	}

	if (i == h->used_pools) {
		if (h->used_pools == h->num_pools) {
			/* we should never be here */
			dbg_assert(false);
			return NULL;
		}

		/* use a new single pool */
		h->spools[i] = alloc(h->__alloc, h->__parent, sizeof(struct heap_spool));
		dbg_assert(h->spools[i] != NULL);
		heap_spool_init(h->spools[i], h->__parent, h->__alloc, h->__dealloc, size);
		h->used_pools ++;

		mem = heap_spool_alloc_c(h->spools[i], size);
		dbg_assert(mem != NULL);
	}
	else {
		/* found one */
		mem = heap_spool_alloc_c(h->spools[i], size);
		dbg_assert(mem != NULL);

		/* try to swap the link to accelerate the query speed */
		if (i > 0) {
			if (h->spools[i]->alloc_count > h->spools[i-1]->alloc_count) {
				struct heap_spool* temp = h->spools[i];
				h->spools[i]   = h->spools[i-1];
				h->spools[i-1] = temp;
			}
		}
	}

	h->alloc_count ++;

	/* attach the size info */
	*(int*)mem = __size;
	mem = (void*)((char*)mem + sizeof(int));

	return mem;
}

bool heap_mpool_dealloc_c(struct heap_mpool* h, void* buff) {
	void* mem = (void*)((int*)buff - 1);
	int size = (int)mem;
	int i, res;

	for (i = 0; i < h->used_pools; i ++) {
		if (h->spools[i]->target_size == size) {
			break;
		}
	}

	dbg_assert(i != h->used_pools);
	if (i == h->used_pools) {
		return false;
	}

	res = heap_spool_dealloc_c(h->spools[i], mem);
	dbg_assert(res == true);

	/* try to swap link to accelerate */
	if (i + 1 < h->used_pools) {
		if (h->spools[i]->alloc_count > h->spools[i + 1]->alloc_count) {
			struct heap_spool* temp = h->spools[i];
			h->spools[i] = h->spools[i+1];
			h->spools[i+1] = temp;
		}
	}

	return res;
}

#else 

void* heap_mpool_alloc_v(struct heap_mpool* h, int __size, const char* file, int line) {
	void* mem;
	int i = 0;
	/* for mpool we must know the size of the buff to deside which spool to dealloc */
	int size = __size + sizeof(int);

	for (i = 0; i < h->used_pools; i ++) {
		if (h->spools[i]->target_size == size) {
			break;
		}
	}

	if (i == h->used_pools) {
		if (h->used_pools == h->num_pools) {
			/* we should never be here */
			dbg_assert(false);
			return NULL;
		}

		/* use a new single pool */
		h->spools[i] = (struct heap_spool*)alloc(h->__alloc, h->__parent, sizeof(struct heap_spool));
		dbg_assert(h->spools[i] != NULL);
		heap_spool_init(h->spools[i], h->__parent, h->__alloc, h->__dealloc, size);
		h->used_pools ++;

		mem = heap_spool_alloc_v(h->spools[i], size, file, line);
		dbg_assert(mem != NULL);
	}
	else {
		/* found one */
		mem = heap_spool_alloc_v(h->spools[i], size, file, line);
		dbg_assert(mem != NULL);

		/* try to swap the link to accelerate the query speed */
		if (i > 0) {
			if (h->spools[i]->alloc_count > h->spools[i-1]->alloc_count) {
				struct heap_spool* temp = h->spools[i];
				h->spools[i]   = h->spools[i-1];
				h->spools[i-1] = temp;
			}
		}
	}

	h->alloc_count ++;

	/* attach the size info */
	*(int*)mem = __size;
	mem = (void*)((char*)mem + sizeof(int));

	return mem;
}

bool heap_mpool_dealloc_v(struct heap_mpool* h, void* buff, const char* file, int line) {
	void* mem = (void*)((int*)buff - 1);
	int size = (int)mem;
	int i, res;

	for (i = 0; i < h->used_pools; i ++) {
		if (h->spools[i]->target_size == size) {
			break;
		}
	}

	dbg_assert(i != h->used_pools);
	if (i == h->used_pools) {
		return false;
	}

	res = heap_spool_dealloc_v(h->spools[i], mem, file, line);
	dbg_assert(res == true);

	/* try to swap link to accelerate */
	if (i + 1 < h->used_pools) {
		if (h->spools[i]->alloc_count > h->spools[i + 1]->alloc_count) {
			struct heap_spool* temp = h->spools[i];
			h->spools[i] = h->spools[i+1];
			h->spools[i+1] = temp;
		}
	}

	return res;
}

#endif
