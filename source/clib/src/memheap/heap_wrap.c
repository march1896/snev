#include <heap_wrap.h>

void heap_wrap_init(struct heap_wrap* h) {
	list_init(&h->allocated);
}

void heap_wrap_deinit(struct heap_wrap* h) {
	/* should we free all memory in the allocated list */
	list_foreach_v();
}

void* heap_wrap_alloc(struct heap_wrap* h, int size) {
	struct block_c* 
}

void* heap_wrap_alloc_v(struct heap_wrap* h, int size, const char* file ,int line) {
}

bool  heap_wrap_dealloc (struct heap_wrap* h, void* buff) {
}

bool  heap_wrap_dealloc_v(struct heap_wrap* h, void* buff, const char* file, int line) {
}

struct block_header {
	struct list_link    link;

	unsigned int        flag;

	struct heap_system* onwer;
};

struct block_footer {
	unsigned int        flag;

	struct heap_system* onwer;
};
#ifdef _MEM_DEBUG_
#endif

void* wheap_alloc(heap_handle h, int size) {

}

void* wheap_alloc_v(heap_handle h, int size, const char* file ,int line) {
}


void  wheap_dealloc(heap_handle h, void* buff) {
}

void  wheap_walk(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb) {
}

void  wheap_walk_v(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param) {
}

heap_handle wheap_spawn(heap_handle parent, pf_alloc parent_alloc, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc) {
	struct heap_wrapped* n_heap = NULL;
	/* we prefer to use verbosed alloc since this will give parent heap more information */
	if (pf_alloc_v != NULL) {
		n_heap = (struct heap_wrapped*)pf_alloc_v(parent, sizeof(struct heap_wrapped), __FILE__, __LINE__);
	}
	else {
		dbg_assert(parent_alloc != NULL);
		n_heap = (struct heap_wrapped*)pf_alloc(parent, sizeof(struct heap_wrapped));
	}
	wheap_init(n_heap, parent_alloc, parent_alloc_v, parent_dealloc, parent);

	return (heap_handle)n_heap;
}
void  wheap_join(heap_handle child) {
	struct heap_wrapped* heap = (struct heap_wrapped*)child;
	pf_dealloc parent_dealloc = heap->__dealloc;
	wheap_deinit(child);

	dbg_assert(parent_dealloc != NULL);
	parent_dealloc(heap);
}

void wheap_init(heap_handle h, pf_alloc parent_alloc, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc, heap_handle parent) {
	struct heap_wrapped* heap = (struct heap_wrapped*)h;

	heap->parent    = parent;
	heap->__alloc   = mem_increase;
	heap->__dealloc = mem_decrease;

	link_init(&heap->allocated);
	link_init(&heap->memories);

	return (heap_handle)heap;
}

void  wheap_deinit(heap_handle h) {
}


