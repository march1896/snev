#include <heap.h>

#include <stdlib.h>

heap_handle sysheap_init(void *buff, int size) {
	UNUSED(buff);
	UNUSED(size);
}

void  sysheap_destroy(heap_handle hhdl) {
	UNUSED(hhdl);
}

void* sysheap_alloc(heap_handle hhdl, int size) {
	UNUSED(hhdl);
	return malloc(size);
}

#ifdef _MEM_DEBUG_
void* sysheap_alloc_debug(heap_handle hhdl, int size, const char* file, int line) {
	/* TODO: we could add debug infomation simplely in front of the allocated memory */
	UNUSED(hhdl);
	UNUSED(file);
	UNUSED(line);

	return malloc(size);
}
#endif

void  sysheap_dealloc(heap_handle hhdl, void *buff) {
	UNUSED(hhdl);
	free(buff);
}

void sysheap_dump(heap_handle hhdl) {
	/* TODO: not supported yet */
	UNUSED(hhdl);
}

#ifdef _MEM_DEBUG_
void sysheap_debug_leak(heap_handle hhdl) {
	/* TODO: not supported yet */
	UNUSED(hhdl);
}

#endif 

