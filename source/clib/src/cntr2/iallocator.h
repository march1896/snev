#ifndef _INTERFACE_ALLOCATOR_H_
#define _INTERFACE_ALLOCATOR_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

#include <memheap/heap_def.h>

extern inline void iallocator_join         (iobject* i); 
/* once you get on the boat, you will never get off */
#ifdef _VERBOSE_ALLOC_DEALLOC_
extern inline void* iallocator_alloc_v     (iobject* i, int size, const char* file, int line);
extern inline bool  iallocator_dealloc_v   (iobject* i, void* buff, const char* file, int line);
#else 
extern inline void* iallocator_alloc_c     (iobject* i, int size);
extern inline bool  iallocator_dealloc_c   (iobject* i, void* buff);
#endif
extern inline object* iallocator_get_parent(iobject* i);
//extern inline void iallocator_walk       (iobject* i, pf_process_block per_block_cb, void* param);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define iallocator_alloc(i, size) iallocator_alloc_v(i, size, __FILE__, __LINE__)
#define iallocator_dealloc(i, buff) iallocator_dealloc_v(i, buff, __FILE__, __LINE__)
#else 
#define iallocator_alloc(i, size) iallocator_alloc_c(i, size)
#define iallocator_dealloc(i, buff) iallocator_dealloc_c(i, buff)
#endif
/* 
 * we don't need to define the interfaces(virtual table function types), since it's defined 
 * in heap_def.h.
 * TODO: should we define the interfaces here?
 * this is a philosophic problem, since it depends on that this project is fully object-oriented or not
 */
typedef void   (*pf_iallocator_join)       (object* o);
typedef void*  (*pf_iallocator_alloc_v)    (object* o, int size, const char* file, int line);
typedef bool   (*pf_iallocator_dealloc_v)  (object* o, void* buff, const char* file, int line);
typedef void*  (*pf_iallocator_alloc_c)    (object* o, int size);
typedef bool   (*pf_iallocator_dealloc_c)  (object* o, void* buff);
typedef object*(*pf_iallocator_get_parent) (object* o);
//typedef void   (*pf_iallocator_walk)       (object* o, pf_process_block per_block_cb, void* param);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define pf_iallocator_alloc pf_iallocator_alloc_v
#define pf_iallocator_dealloc pf_iallocator_dealloc_v
#else 
#define pf_iallocator_alloc pf_iallocator_alloc_c
#define pf_iallocator_dealloc pf_iallocator_dealloc_c
#endif

struct iallocator_vtable {
	/* heap doest not contains destroy method */
	pf_iallocator_join           __join;

	pf_iallocator_alloc          __alloc;
	pf_iallocator_dealloc        __dealloc;

	pf_iallocator_get_parent     __get_parent;
	//pf_iallocator_walk           __heap_walk;
};

#endif /* _INTERFACE_ALLOCATOR_H_ */
