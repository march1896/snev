#ifndef _CORE_HEAP_H_
#define _CORE_HEAP_H_

/* this is the internal interface for memory management, you can implement 
 * these functions to provide a way to managing memory */
typedef void* heap_handle;

/* init a heap on a buffer of a given size */
typedef heap_handle (*pf_heap_init)(void *buff, int size);

/* deinit a heap on a given buffer */
typedef void (*pf_heap_destroy)(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
typedef void* (*pf_heap_alloc)(heap_handle pheap, int size);

/* dealloca a buff from a heap */
typedef void (*pf_heap_dealloc)(heap_handle pheap, void *buff);

/*  reallocate the buff to given size */
typedef void (*pf_heap_realloc)(heap_handle pheap, void *buff, int size);

struct heap_operations {
	pf_heap_init 	init;
	pf_heap_destroy	destroy;
	pf_heap_alloc	alloc;
	pf_heap_dealloc	dealloc;
	pf_heap_realloc	realloc;
};

#endif // _CORE_HEAP_H_
