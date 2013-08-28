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

/* single pool store fix sized blocks */
struct heap_spool {
	void*            __parent;
	pf_alloc         __alloc;
	pf_dealloc       __dealloc;

	int              target_size;     /* the target size, which will pooled in */
	int              alloc_count;     /* number of allocated block in this pool */
	int              start_level;     
	int              level;           /* level is the current storage level */

	void*            next;
	struct list_link sentinel;
};

#define HEAP_POOL_START_LEVEL 4
#define _HEAP_POOL_DEBUG_CHECK_

/* multiple pool store multiple different sized blocks */
struct heap_mpool {
	void*            __parent;
	pf_alloc         __alloc;
	pf_dealloc       __dealloc;

	int              num_pools;
	int              used_pools;
	struct heap_spool** spools;

	int              alloc_count;
};

/* define the functions inside the macro make us available keep the file/line information 
 * of original invoke */
#ifdef _VERBOSE_ALLOC_DEALLOC_
void* heap_spool_alloc_v (struct heap_spool* h, int size, const char* file ,int line);
bool  heap_spool_dealloc_v(struct heap_spool* h, void* buff, const char* file, int line);
#define heap_spool_alloc   heap_spool_alloc_v
#define heap_spool_dealloc heap_spool_dealloc_v

void* heap_mpool_alloc_v (struct heap_mpool* h, int size, const char* file ,int line);
bool  heap_mpool_dealloc_v(struct heap_mpool* h, void* buff, const char* file, int line);
#define heap_mpool_alloc   heap_mpool_alloc_v
#define heap_mpool_dealloc heap_mpool_dealloc_v
#else 
void* heap_spool_alloc_c (struct heap_spool* h, int size);
bool  heap_spool_dealloc_c(struct heap_spool* h, void* buff);
#define heap_spool_alloc   heap_spool_alloc_c
#define heap_spool_dealloc heap_spool_dealloc_c

void* heap_mpool_alloc_c (struct heap_mpool* h, int size);
bool  heap_mpool_dealloc_c(struct heap_mpool* h, void* buff);
#define heap_mpool_alloc   heap_mpool_alloc_c
#define heap_mpool_dealloc heap_mpool_dealloc_c
#endif

void  heap_spool_init    (struct heap_spool* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc);
void  heap_spool_deinit  (struct heap_spool* h);

void  heap_mpool_init    (struct heap_mpool* h, void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int max_diff_type);
void  heap_mpool_deinit  (struct heap_mpool* h);

#endif /* _HEAP_POOLED_SYSTEM_H_*/
