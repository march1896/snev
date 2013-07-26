#include <heap.h>

#include <stdlib.h>

void* sheap_alloc(heap_handle h, int size) {
	unused(h);

	return malloc(size);
}

void* sheap_alloc_v(heap_handle h, int size, const char* file ,int line) {
	unused(h);
	unused(file);
	unused(line);

	return malloc(size);
}

void sheap_dealloc(heap_handle h, void* buff) {
	/* we can not handle the system heap, the only way to use it is globally */
	unused(h);

	free(buff);
}

void sheap_walk(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb) {
	/* this method is not supported by the system heap */
	unused(h);
	unused(allocated_cb);
	unused(freed_cb);

	dbg_assert(false);
}

void sheap_walk_v(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb, void* param) {
	/* this method is not supported by the system heap */
	unused(h);
	unused(allocated_cb);
	unused(freed_cb);
	unused(param);

	dbg_assert(false);
}

