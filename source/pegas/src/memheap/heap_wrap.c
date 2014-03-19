#include <memheap/heap_wrap.h>

void  heap_wrap_init(struct heap_wrap* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	h->__parent  = __parent;
	h->__parent_alloc   = __alloc;
	h->__parent_dealloc = __dealloc;

	list_init(&h->allocated);
}

void heap_wrap_deinit(struct heap_wrap* h) {
	/* should we free all memory in the allocated list? */
	/* TODO: */
	//list_foreach_v();
}

#ifndef _VERBOSE_ALLOC_DEALLOC_
void* heap_wrap_alloc_c(struct heap_wrap* h, int size) {
	int __size = size + sizeof(struct listlink);
	void* buff = alloc(h->__parent_alloc, h->__parent, size);

	dbg_assert(buff != NULL);
	/* link the allocated block in to list */
	{
		struct listlink* n_link = (struct listlink*)buff;
		list_insert_back(&h->allocated, n_link);
	}

	/* return the 'real' address */
	return (char*)buff + sizeof(struct listlink);
}

bool  heap_wrap_dealloc_c(struct heap_wrap* h, void* buff) {
	/* first get the real address of the block */
	void* real_addr = (char*)buff - sizeof(struct listlink);
	struct listlink* link = (struct listlink*)real_addr;

	/* remove it from the allocated list */
	list_remove(&h->allocated, link);

	/* return the memory to the 'real' heap */
	return h->__parent_dealloc(h->__parent, real_addr);
	/* using the raw __dealloc to keep the client file & line */
	/*dealloc(h->__dealloc, h->__parent);*/
}

#else /* #defined _VERBOSE_ALLOC_DEALLOC_ */

void* heap_wrap_alloc_v(struct heap_wrap* h, int size, const char* file ,int line) {
	int __size = size + sizeof(struct listlink);
	//void* buff = alloc(h->__alloc, h->__parent, __size);
	void* buff = h->__parent_alloc(h->__parent, __size, file, line);

	dbg_assert(buff != NULL);
	/* link the allocated block in to list */
	{
		struct listlink* n_link = (struct listlink*)buff;
		list_insert_back(&h->allocated, n_link);
	}

	/* return the 'real' address */
	buff = (void*)((char*)buff + sizeof(struct listlink));
	return buff;
}


bool  heap_wrap_dealloc_v(struct heap_wrap* h, void* buff, const char* file, int line) {
	/* first get the real address of the block */
	void* real_addr = (char*)buff - sizeof(struct listlink);
	struct listlink* link = (struct listlink*)real_addr;

	/* remove it from the allocated list */
	list_remove(&h->allocated, link);

	/* using the raw __dealloc to keep the client file & line */
	/*dealloc(h->__dealloc, h->__parent, real_addr, file, line);*/
	return h->__parent_dealloc(h->__parent, real_addr, file, line);
}

#endif

