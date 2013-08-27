#ifndef _HEAP_POOLED_SYSTEM_H_
#define _HEAP_POOLED_SYSTEM_H_

#include "memheap/heap_def.h"
#include "util/list_link.h"

/* Implement the following interface one by one */
/* the below interface should be implemented, from heap_def.h
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);
*/

struct heap_pool_single {
	void*            __parent;
	pf_alloc         __alloc;
	pf_dealloc       __dealloc;

	int              psize;     /* the pooled memory size */
	int              count;     /* number of allocated block in this pool */
	int              level;     /* level of totally buffered memory in this single pool */

	void*            next;
	struct list_link sentinel;
};

struct heap_pool_multiple {
	void*            __parent;
	pf_alloc         __alloc;
	pf_dealloc       __dealloc;

	int              num_pools;
	int              used_pools;
	struct heap_pool_single* pools;
};

/* define the functions inside the macro make us available keep the file/line information 
 * of original invoke */
#ifdef _VERBOSE_ALLOC_DEALLOC_
void* heap_pool_alloc_v (struct heap_pool* h, int size, const char* file ,int line);
bool  heap_pool_dealloc_v(struct heap_pool* h, void* buff, const char* file, int line);
#define heap_pool_alloc   heap_pool_alloc_v
#define heap_pool_dealloc heap_pool_dealloc_v
#else 
void* heap_pool_alloc_c (struct heap_pool* h, int size);
bool  heap_pool_dealloc_c(struct heap_pool* h, void* buff);
#define heap_pool_alloc   heap_pool_alloc_c
#define heap_pool_dealloc heap_pool_dealloc_c
#endif

void  heap_pool_init    (struct heap_pool* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc);
void  heap_pool_deinit  (struct heap_pool* h);


#endif /* _HEAP_POOLED_SYSTEM_H_*/
