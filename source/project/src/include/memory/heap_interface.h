#ifndef _CORE_HEAP_H_
#define _CORE_HEAP_H_

/* this is the internal interface for memory management, you can implement 
 * these functions to provide a way to managing heap memory, but it just provide 
 * the functionality of memory management, it can not make any assumption about 
 * how client will use heap. */

/* if this macro is defined, that means we should provide some method to record 
 * allocate/deallocate operations and provide way to dump debug information */

#define MEMORY_DEBUG

typedef void* heap_handle;

#ifndef MEMORY_DEBUG
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
typedef void* (*pf_heap_realloc)(heap_handle pheap, void *buff, int size);

/* dump debug information of the heap */
typedef void (*pf_heap_dump)(heap_handle pheap);

#else // defined MEMORY_DEBUG

/* init a heap on a buffer of a given size */
typedef heap_handle (*pf_heap_init_debug)(void *buff, int size);

/* deinit a heap on a given buffer */
typedef void (*pf_heap_destroy_debug)(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
typedef void* (*pf_heap_alloc_debug)(heap_handle pheap, int size, const char* file, size_t line);

/* dealloca a buff from a heap */
typedef void (*pf_heap_dealloc_debug)(heap_handle pheap, void *buff, const char* file, size_t line);

/*  reallocate the buff to given size */
typedef void* (*pf_heap_realloc_debug)(heap_handle pheap, void *buff, int size, const char* file, size_t line);

/* dump debug information of the heap */
typedef void (*pf_heap_dump_debug)(heap_handle pheap);

#endif // MEMORY_DEBUG

/* typically, we need the following information 
 * 1, total allocated memory block number
 * 2, total allocated memory size
 * 3, total free memory block number
 * 4, total free memory size
 * 5, largest free memory block size
 * 6, allocated(not released yet) memory info
 * 7, mis-deleted memory info
 * this helper function helps you to dump the first five 
 * information with format 
 * all inputs must be nonnegative, -1 means specific record is unknown */
void heap_dump_helper(int tot_alloc_number, int tot_alloc_size, 
		int tot_free_number, int tot_free_size, int largest_free_size);

struct heap_operations {
#ifndef MEMORY_DEBUG
	pf_heap_init 	init;
	pf_heap_destroy	destroy;
	pf_heap_alloc	alloc;
	pf_heap_dealloc	dealloc;
	pf_heap_realloc	realloc;
	pf_heap_dump	dump;
#else
	pf_heap_init_debug 		init;
	pf_heap_destroy_debug	destroy;
	pf_heap_alloc_debug 	alloc;
	pf_heap_dealloc_debug 	dealloc;
	pf_heap_realloc_debug 	realloc;
	pf_heap_dump_debug 		dump;
#endif
};

#endif // _CORE_HEAP_H_
