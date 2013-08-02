#include <iheap.h>

#include <memheap/heap_def.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_factory.h>

#include <imemmgr.h>
#include <util/list_link.h>

enum heap_interfaces {
	e_heap,
	e_heap_count
};

struct o_heap_llrb {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_heap_count];

	struct heap_llrb              __driver_heap;
};

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

static inline void o_heap_llrb_join() {
}
static inline void o_heap_llrb_alloc();
static inline void o_heap_llrb_dealloc();
static inline void o_heap_llrb_get_blockinfo();
static inline void o_heap_llrb_heap_walk();

static struct iheap_vtable __iheap_llrb_vtable = {
	heap_llrb_join,           /* __join */
	heap_llrb_alloc,          /* __alloc */
	heap_llrb_dealloc,        /* __dealloc */
	heap_llrb_get_blockinfo,  /* __get_blockinfo */
	heap_llrb_heap_walk,      /* __heap_walk */
};

static unknown o_heap_llrb_cast(unknown x, unique_id intf_id);

static unknown o_heap_llrb_cast(unknown x, unique_id intf_id) {
	struct o_dlist* o = (struct o_dlist*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IHEAP_ID:
		return (unknown)&o->__iftable[e_heap];
	default:
		return NULL;
	}

	return NULL;
}

static object* o_heap_llrb_spawn(iobject* __parent) {
	struct o_heap_llrb* oheap = NULL;
	pf_alloc __parent_alloc = NULL;
	pf_dealloc __parent_dealloc = NULL;

	dbg_assert(__parent != NULL);

	oheap = iheap_alloc(__parent, sizeof(struct o_heap_llrb));

	oheap->__offset = oheap;
	olist->__cast   = o_heap_llrb_cast;
	
	olist->__iftable[e_list].__offset = (address)e_heap;
	olist->__iftable[e_list].__vtable = &__iheap_llrb_vtable;

	{
		/* this is a little trick, get the parent's virtual function. */
		/* C++ does not allow to use object's virtual function as callback(only static function can be used),
		 * here we do so, is because of the change from procedure world to oo world */
		object* oparent = __object_from_interface(__parent);
		dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

		__parent_alloc = ((struct iheap_vtable*)iheap->__vtable)->__alloc;
		__parent_dealloc = ((struct iheap_vtable*)iheap->__vtable)->__dealloc; 
	}
	/* initialize the heap llrb driver */
	/* TODO when we can get the heap's property from interface, we should use the _v init*/
	heap_llrb_init(&oheap->__driver_heap, __parent, __parent_alloc, __parent_dealloce);

	return (object*)olist;
}
