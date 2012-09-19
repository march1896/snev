#ifndef _HEAP_H_
#define _HEAP_H_

#include <cominc.h>

typedef void* heap_handle;

/* init a heap on a buffer of a given size */
heap_handle heap_init(void *buff, int size);

/* deinit a heap on a given buffer */
void heap_destroy(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* heap_alloc(heap_handle pheap, int size, const char* file, size_t line);

/* dealloca a buff from a heap */
void heap_dealloc(heap_handle pheap, void *buff, const char* file, size_t line);

/*  reallocate the buff to given size */
void* heap_realloc(heap_handle pheap, void *buff, int size, const char* file, size_t line);

/* dump debug information of the heap */
void heap_dump(heap_handle pheap);

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

#endif /* _HEAP_H_ */
