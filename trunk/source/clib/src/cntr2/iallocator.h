#ifndef _INTERFACE_ALLOCATOR_H_
#define _INTERFACE_ALLOCATOR_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

#include <memheap/heap_def.h>

/* the interfaces here is not clear enough because of macros, see oallocator.h for more details */

extern inline void  iallocator_join        (iobject* i); 
/* once you get on the boat, you will never get off */
#ifdef _VERBOSE_ALLOC_DEALLOC_
extern inline void* iallocator_acquire_v   (iobject* i, int size, const char* file, int line);
extern inline bool  iallocator_release_v   (iobject* i, void* buff, const char* file, int line);
#define pf_iallocator_acquire pf_iallocator_acquire_v
#define pf_iallocator_release pf_iallocator_release_v
#else 
extern inline void* iallocator_acquire_c   (iobject* i, int size);
extern inline bool  iallocator_release_c   (iobject* i, void* buff);
#define pf_iallocator_acquire pf_iallocator_acquire_c
#define pf_iallocator_release pf_iallocator_release_c
#endif
extern inline object* iallocator_get_parent(iobject* i);
//extern inline void iallocator_walk       (iobject* i, pf_process_block per_block_cb, void* param);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define iallocator_alloc(i, size) iallocator_acquire_v(i, size, __FILE__, __LINE__)
#define iallocator_dealloc(i, buff) iallocator_release_v(i, buff, __FILE__, __LINE__)
#else 
#define iallocator_alloc(i, size) iallocator_acquire_c(i, size)
#define iallocator_dealloc(i, buff) iallocator_release_c(i, buff)
#endif

/* codes below is only useful for allocator implementors */
typedef void   (*pf_iallocator_join)       (object* o);
typedef void*  (*pf_iallocator_acquire_v)  (object* o, int size, const char* file, int line);
typedef bool   (*pf_iallocator_release_v)  (object* o, void* buff, const char* file, int line);
typedef void*  (*pf_iallocator_acquire_c)  (object* o, int size);
typedef bool   (*pf_iallocator_release_c)  (object* o, void* buff);
typedef object*(*pf_iallocator_get_parent) (object* o);
//typedef void   (*pf_iallocator_walk)       (object* o, pf_process_block per_block_cb, void* param);

struct iallocator_vtable {
	/* heap doest not contains destroy method */
	pf_iallocator_join           __join;

	pf_iallocator_acquire        __acquire;
	pf_iallocator_release        __release;

	pf_iallocator_get_parent     __get_parent;
	//pf_iallocator_walk           __heap_walk;
};

#endif /* _INTERFACE_ALLOCATOR_H_ */
