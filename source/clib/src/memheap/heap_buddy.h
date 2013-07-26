#ifndef _HEAP_BUDDY_H_
#define _HEAP_BUDDY_H_

#include <heap_def.h>

/* init a heap on a buffer of a given size */
heap_handle bheap_init(void *buff, int size);

//heap_handle bheap_init(pf_mem_increase cb_inc, pf_mem_decrease cn_dec);

/* deinit a heap on a given buffer */
void bheap_destroy(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* bheap_alloc(heap_handle pheap, int size);

/* dealloca a buff from a heap */
void bheap_dealloc(heap_handle pheap, void *buff);

/* dump debug information of the heap */
void bheap_dump(heap_handle pheap);

#ifdef _MEM_DEBUG_
void bheap_debug_leak(heap_handle hhdl);
#endif

void bheap_init_global(int size);

void bheap_deinit_global();

void bheap_debug_global_leak();

extern heap_handle bheap_global_handle;

/* typedef void* (*pf_alloc)  (heap_handle h, int size); */
void* bheap_alloc(heap_handle pheap, int size);

/* typedef void* (*pf_alloc_v)(heap_handle h, int size, const char* file, int line); */
void* bheap_alloc_v(heap_handle hhdl, int size, const char* file, int line);

/* typedef void* (*pf_alloc_g) (int size); */
inline void* bheap_alloc_g(int size) {
	bheap_alloc(bheap_global_handle, size);
}

/* typedef void* (*pf_alloc_gv)(int size, const char* file, int line); */
inline void* bheap_alloc_gv(int size, const char* file, int line) {
	bheap_alloc_v(bheap_global_handle, size, file, size);
}

/* typedef void (*pf_dealloc)  (heap_handle h, void* buff); */
void bheap_dealloc(heap_handle pheap, void *buff);

/* typedef void (*pf_dealloc_g)(void* buff); */
void bheap_dealloc_g(void* buff);

/*
typedef void (*pf_mem_process)(void* mem);
typedef void (*pf_heap_walk)(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
*/
void bheap_walk(heap_handle h, pf_mem_process allocated_cb, pf_mem_process freed_cb);

typedef void* (*pf_mem_increase)(int size);
typedef void (*pf_heap_init)(heap_handle h, pf_mem_increase increase_cb);
typedef void (*pf_heap_deinit)(heap_handle h);

#endif /* _HEAP_BUDDY_H_ */
