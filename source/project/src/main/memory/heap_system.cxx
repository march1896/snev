#include <heap_system.h>

#include <stdlib.h>

heap_handle cb_heap_init_sys(void *buff, int size) {
	return (heap_handle) NULL;
}

void cb_heap_destroy_sys(heap_handle pheap) {
	return;
}

void* cb_heap_alloc_sys(heap_handle pheap, int size) {
	return malloc((size_t)size);
}

void cb_heap_dealloc_sys(heap_handle pheap, void *buff) {
	free(buff);
}

void cb_heap_realloc_sys(heap_handle pheap, void *buff, int size) {
	realloc(buff, size);
}
