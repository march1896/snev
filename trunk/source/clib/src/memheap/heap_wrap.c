#include <heap_wrap.h>

void  heap_wrap_init(struct heap_wrap* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	h->__parent  = __parent;
	h->__alloc   = __alloc;
	h->__dealloc = __dealloc;

	list_init(&h->allocated);
}

void heap_wrap_deinit(struct heap_wrap* h) {
	/* should we free all memory in the allocated list */
	/* TODO: */
	list_foreach_v();
}

void* heap_wrap_alloc_c(struct heap_wrap* h, int size) {
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

void* heap_wrap_alloc_v(struct heap_wrap* h, int size, const char* file ,int line) {
	int __size = size + sizeof(struct list_link);
	void* buff = alloc(h->__alloc, h->__parent, __size, file, line);

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

bool  heap_wrap_dealloc_c(struct heap_wrap* h, void* buff) {
	/* first get the real address of the block */
	void* real_addr = (char*)buff - sizeof(struct list_link);
	struct list_link* link = (struct list_link*)real_addr;

	/* remove it from the allocated list */
	list_remove(&h->allocated, link);

	/* return the memory to the 'real' heap */
	h->__dealloc(h->__parent, real_addr);
	/* using the raw __dealloc to keep the client file & line */
	/*dealloc(h->__dealloc, h->__parent);*/
}

bool  heap_wrap_dealloc_v(struct heap_wrap* h, void* buff, const char* file, int line) {
	/* first get the real address of the block */
	void* real_addr = (char*)buff - sizeof(struct list_link);
	struct list_link* link = (struct list_link*)real_addr;

	/* remove it from the allocated list */
	list_remove(&h->allocated, link);

	/* return the memory to the 'real' heap */
	dealloc(h->__dealloc, h->__parent, real_addr, file, line);
	/* using the raw __dealloc to keep the client file & line */
	/*dealloc(h->__dealloc, h->__parent, real_addr);*/
}

void  heap_wrap_walk(struct heap_wrap* h, pf_mem_process allocated_cb, pf_mem_process freed_cb) {
	/* TODO: we should know the walk function of the parent heap */
}
void  heap_wrap_walk_v  (struct heap_wrap* h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param) {
	/* TODO: we should know the walk function of the parent heap */
}