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

	struct heap_llrb              __driver;
};

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

	dbg_assert(__parent != NULL);

	oheap = iheap_alloc(__parent, sizeof(struct o_heap_llrb));

	oheap->__offset = oheap;
	olist->__cast   = o_heap_llrb_cast;
	
	olist->__iftable[e_list].__offset = (address)e_heap;
	olist->__iftable[e_list].__vtable = &__iheap_llrb_vtable;

	/* initialize the heap llrb driver */
	/* TODO */

	return (object*)olist;
}
