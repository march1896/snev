#ifndef _INTERFACE_HEAP_ALLOCATOR_H_
#define _INTERFACE_HEAP_ALLOCATOR_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

#include <heap_def.h>

extern inline void iheap_join         (iobject* iheap); 
/* once you get on the boat, you will never get off */
#ifdef _VERBOSE_ALLOC_DEALLOC_
extern inline void iheap_alloc_v      (iobject* iheap, int size, const char* file, int line);
extern inline void iheap_dealloc_v    (iobject* iheap, void* buff, const char* file, int line);
#else 
extern inline void iheap_alloc_c      (iobject* iheap, int size);
extern inline void iheap_dealloc_c    (iobject* iheap, void* buff);
#endif
extern inline void iheap_get_blockinfo(iobject* iheap, void* mem_addr, struct heap_blockinfo* binfo);
extern inline void iheap_walk         (iobject* iheap, pf_process_block per_block_cb, void* param);

#ifdef _VERBOSE_ALLOC_DEALLOC_
#define iheap_alloc(iheap, size) iheap_alloc_v(iheap, size, __FILE__, __LINE__)
#define iheap_dealloc(iheap, buff) iheap_dealloc_v(iheap, buff, __FILE__, __LINE__)
#else 
#define iheap_alloc(iheap, size) iheap_alloc_c(iheap, size)
#define iheap_dealloc(iheap, buff) iheap_dealloc_c(iheap, buff)
#endif
/* 
 * we don't need to define the interfaces(virtual table function types), since it's defined 
 * in heap_def.h.
 * TODO: should we define the interfaces here?
 * this is a philosophic problem, since it depends on that this project is fully object-oriented or not
 */
typedef void        (*pf_iheap_join)     (object* pheap);
typedef void*       (*pf_iheap_alloc_c)  (object* pheap, int size);
typedef void*       (*pf_iheap_alloc_v)  (object* pheap, int size, const char* file, int line);
typedef bool        (*pf_iheap_dealloc_c)(object* pheap, void* buff);
typedef bool        (*pf_iheap_dealloc_v)(object* pheap, void* buff, const char* file, int line);
typedef void        (*pf_iheap_get_blockinfo)(object* pheap, void* mem_addr, /* out */ struct heap_blockinfo* info);
typedef void        (*pf_iheap_walk)

struct iheap_vtable {
	/* heap doest not contains destroy method */
	pf_heap_join            __join;

	pf_alloc                __alloc;
	pf_dealloc              __dealloc;

	pf_get_blockinfo        __get_blockinfo;

	pf_heap_walk            __heap_walk;
};

#endif /* _INTERFACE_HEAP_ALLOCATOR_H_ */
