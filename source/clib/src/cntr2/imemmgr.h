#ifndef _INTERFACE_MEMORY_MANAGEMENT_H_
#define _INTERFACE_MEMORY_MANAGEMENT_H_
/* TODO: remove this file, no longer needed */

#include <cominc.h>
#include <oo_model.h>
#include <memheap/heap_def.h>
//#include <heap_global.h>
#include <memheap/heap_sys.h>

/* definitions in heap_def.h
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);
*/

/* below are the lifetime management part, it not an interface of the heap. 
 * Just like factory, the spawn/join manages the create/destroy of an object, 
 * but more than factory, it does dot creates the objects as a set, but organize 
 * them in a tree */
typedef unknown     (*pf_heap_spawn)(unknown parent, pf_alloc parent_alloc, pf_alloc_v parent_alloc_v, pf_dealloc parent_dealloc);

typedef void        (*pf_dispose)   (void* buff);
typedef void*       (*pf_copy   )   (void* buff);

extern void* cntr_default_heap;
#define cntr_default_alloc   heap_sysd_alloc
#define cntr_default_dealloc heap_sysd_dealloc

#endif /* _INTERFACE_MEMORY_MANAGEMENT_H_ */
