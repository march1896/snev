#include <heap.h>

struct heap_common {
	heap_operations __vt;
};

void heap_destroy(heap_handle pheap) {
	struct heap_common *phc = (struct heap_common*)pheap;

	phc->__vt->destroy();
}

void* heap_alloc(heap_handle pheap, int size, const char* file, size_t line) {
	struct heap_common *phc = (struct heap_common*)pheap;

	return phc->__vt->alloc(pheap, size, file, line);
}

void heap_dealloc(heap_handle pheap, void *buff, const char* file, size_t line) {
	struct heap_common *phc = (struct heap_common*)pheap;

	phc->__vt->dealloc(pheap, buff, file, line);
}

void* heap_realloc(heap_handle pheap, void *buff, int size, const char* file, size_t line) {
	struct heap_common *phc = (struct heap_common*)pheap;

	return phc->__vt->realloc(pheap, buff, size, file, line);
}

void heap_dump(heap_handle pheap) {
}


