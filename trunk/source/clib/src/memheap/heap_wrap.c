#include <heap.h>

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

/* should the verbosed alloc binded with _MEM_DEBUG_ ? 
 * temporarily my answer is no */

struct heap_wrapped {
	heap_handle      parent;
	pf_alloc         __grow_alloc;
	/* if the verbosed alloc function is given, we prefer to use it */
	pf_alloc_v       __grow_alloc_v;
	pf_dealloc       __grow_dealloc;

	struct list_link allocated;
	struct list_list memories;
};

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

heap_handle wheap_spawn(heap_handle parent, pf_alloc mem_increase, pf_dealloc mem_decrease);
void  wheap_join(heap_handle child);

void wheap_init(heap_handle h, pf_alloc mem_increase, pf_alloc_v mem_increase_v, pf_dealloc mem_decrease, heap_handle parent) {
	struct heap_wrapped* heap = (struct heap_wrapped*)h;

	heap->parent         = parent;
	heap->__grow_alloc   = mem_increase;
	heap->__grow_alloc_v = mem_increase_v;
	heap->__grow_dealloc = mem_decrease;

	link_init(&heap->allocated);
	link_init(&heap->memories);

	return (heap_handle)heap;
}

void  wheap_deinit(heap_handle h) {
}


