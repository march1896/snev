#ifndef _HEAP_H_
#define _HEAP_H_

#define _MEM_DEBUG_

typedef void* heap_handle;
/* init a heap on a buffer of a given size */
heap_handle heap_init(void *buff, int size);

/* deinit a heap on a given buffer */
void heap_destroy(heap_handle pheap);

/* alloc a buff from a heap, return the address of the allocated heap, 
 * NULL if allocate failed */
void* heap_alloc(heap_handle pheap, int size);

/* dealloca a buff from a heap */
void heap_dealloc(heap_handle pheap, void *buff);

/* dump debug information of the heap */
void heap_dump(heap_handle pheap);

#ifdef _MEM_DEBUG_
void heap_debug_leak(heap_handle hhdl);
void* heap_alloc_debug(heap_handle hhdl, int size, const char* file, int line);
#endif

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


extern heap_handle _global_llrb_heap;

#ifdef _USING_LLRB_HEAP_

#ifdef _MEM_DEBUG_

#define halloc(size) \
	heap_alloc_debug(_global_llrb_heap, size, __FILE__, __LINE__)
#define hfree(buff) \
	heap_dealloc(_global_llrb_heap, buff)

#else 

#define halloc(size) \
	heap_alloc(_global_llrb_heap, size)
#define hfree(buff) \
	heap_dealloc(_global_llrb_heap, buff)

#endif

#else 

#define halloc malloc
#define hfree free

#endif

void heap_init_global(int size);

void heap_deinit_global();

#ifdef _MEM_DEBUG_
void heap_debug_global_leak();
#endif
#endif /* _HEAP_H_ */
