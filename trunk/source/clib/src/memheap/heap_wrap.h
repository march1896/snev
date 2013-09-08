#ifndef _HEAP_WRAPPED_SYSTEM_H_
#define _HEAP_WRAPPED_SYSTEM_H_

#include <memheap/heap_def.h>
#include <util/list_link.h>

/* 
 * This is just a wrap of the system heap, which saves the allocated block in a linked list,
 * which could give more debug information than the raw system heap.
 * Of cource, use more memory to store the debug information 
 */

/* Implement the following interface one by one */
/* the below interface should be implemented, from heap_def.h
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);
*/

/* heap_wrap_node describes an allocated block */
struct heap_wrap_node {
	struct list_link link;

	void*            rel_addr;
};

struct heap_wrap {
	void*            __parent;
	pf_alloc         __parent_alloc;
	pf_dealloc       __parent_dealloc;

	struct list_link allocated;
};

/* define the functions inside the macro make us available keep the file/line information 
 * of original invoke */
#ifdef _VERBOSE_ALLOC_DEALLOC_
void* heap_wrap_alloc_v (struct heap_wrap* h, int size, const char* file ,int line);
bool  heap_wrap_dealloc_v(struct heap_wrap* h, void* buff, const char* file, int line);
#define heap_wrap_alloc   heap_wrap_alloc_v
#define heap_wrap_dealloc heap_wrap_dealloc_v
#else 
void* heap_wrap_alloc_c (struct heap_wrap* h, int size);
bool  heap_wrap_dealloc_c(struct heap_wrap* h, void* buff);
#define heap_wrap_alloc   heap_wrap_alloc_c
#define heap_wrap_dealloc heap_wrap_dealloc_c
#endif

void  heap_wrap_init    (struct heap_wrap* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc);
void  heap_wrap_deinit  (struct heap_wrap* h);


#endif /* _HEAP_WRAPPED_SYSTEM_H_*/
