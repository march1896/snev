#include <string.h>

#include "cntr2/iset.h"
#include "cntr2/imset.h"
#include "cntr2/iitr.h"

#include "cntr2/ifactory.h"
#include "cntr2/oallocator.h"

#include "util/skiplist.h"

/* this module defines a left-lean red black tree container, which implements iset interface. */

enum skiplist_interfaces {
	e_set,
	e_mset,
	e_l_count
};

struct oskiplist_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	iobject                       __iftable[1];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     __allocator;

	const struct skip_link*       __current;
};

/* binary search tree */
struct oskiplist {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	struct skiplist*              __skiplist;

	int                           __size;
	pf_compare                    __ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     __allocator;
	bool                          __allocator_join_ondispose;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_dispose                    __dispose;         

	struct oskiplist_itr          __itr_begin;
	struct oskiplist_itr          __itr_end;
};

static object*  oskiplist_create          (pf_compare ref_comp);
static object*  oskiplist_create_v        (pf_compare ref_comp, allocator alc, pf_dispose dispose);
static void     oskiplist_destroy         (object* o);
static void     oskiplist_clear           (object* o);
static int      oskiplist_size            (const object* o);
static bool     oskiplist_empty           (const object* o);
static bool     oskiplist_insert_s        (object* o, void* __ref);
static void     oskiplist_insert_m        (object* o, void* __ref);
static bool     oskiplist_contains        (const object* o, void* __ref);
static int      oskiplist_count           (const object* o, void* __ref);
static bool     oskiplist_remove          (object* o, void* __ref);

static iterator oskiplist_itr_create      (const object* o, itr_pos pos);
static void     oskiplist_itr_assign      (const object* o, iterator itr, itr_pos pos);
static void     oskiplist_itr_find        (const object* o, iterator itr, void* __ref);
static void     oskiplist_itr_find_lower  (const object* o, iterator itr, void* __ref);
static void     oskiplist_itr_find_upper  (const object* o, iterator itr, void* __ref);

static void*    oskiplist_itr_remove      (object* o, iterator itr);

static const_iterator oskiplist_itr_begin (const object* o);
static const_iterator oskiplist_itr_end   (const object* o);

static struct iset_vtable __iset_vtable = {
	oskiplist_destroy,          /* __destroy */
	oskiplist_clear,            /* __clear */
	oskiplist_size,             /* __size */
	oskiplist_empty,            /* __empty */
	oskiplist_insert_s,         /* __insert */
	oskiplist_contains,         /* __contains */
	oskiplist_remove,           /* __remove */

	oskiplist_itr_create,       /* __itr_create */
	oskiplist_itr_assign,       /* __itr_assign */
	oskiplist_itr_find,         /* __itr_find */
	oskiplist_itr_remove,       /* __itr_remove */
	oskiplist_itr_begin,        /* __itr_begin */
	oskiplist_itr_end           /* __itr_end */
};

static struct imset_vtable __imset_vtable = {
	oskiplist_destroy,          /* __destroy */
	oskiplist_clear,            /* __clear */
	oskiplist_size,             /* __size */
	oskiplist_empty,            /* __empty */
	oskiplist_insert_m,         /* __insert */
	oskiplist_contains,         /* __contains */
	oskiplist_count,            /* __count */
	oskiplist_remove,           /* __remove */

	oskiplist_itr_create,       /* __itr_create */
	oskiplist_itr_assign,       /* __itr_assign */
	oskiplist_itr_find_lower,   /* __itr_find_lower */
	oskiplist_itr_find_upper,   /* __itr_find_upper */
	oskiplist_itr_remove,       /* __itr_remove */
	oskiplist_itr_begin,        /* __itr_begin */
	oskiplist_itr_end           /* __itr_end */
};

static unknown oskiplist_itr_cast(unknown x, unique_id inf_id);
static unknown oskiplist_cast(unknown x, unique_id intf_id);

static void oskiplist_itr_destroy(iterator citr);
static iterator oskiplist_itr_clone(const_iterator citr);
static bool oskiplist_itr_equals(const_iterator a, const_iterator b);
static const void* oskiplist_itr_get_ref(const_iterator citr);
static void oskiplist_itr_set_ref(iterator citr, const void* n_ref);
static void oskiplist_itr_to_next(iterator citr);
static void oskiplist_itr_to_prev(iterator citr);

static struct itr_bidirectional_vtable __itr_vtable = {
	oskiplist_itr_destroy,      /* __destroy */
	oskiplist_itr_clone,        /* __clone   */
	oskiplist_itr_equals,       /* __equals  */
	oskiplist_itr_get_ref,      /* __get_ref */
	oskiplist_itr_set_ref,      /* __set_ref */
	oskiplist_itr_to_next,      /* __to_next */
	oskiplist_itr_to_prev       /* __to_prev */
};


static void oskiplist_itr_destroy(object* citr) {
	struct oskiplist_itr* itr = (struct oskiplist_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oskiplist_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->__allocator, itr);
}

static iterator oskiplist_itr_clone(const_iterator citr) {
	struct oskiplist_itr* itr = (struct oskiplist_itr*)citr;
	struct oskiplist_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oskiplist_itr_cast);

	/* destroy itself */
	n_itr = (struct oskiplist_itr*)allocator_alloc(itr->__allocator, sizeof(struct oskiplist_itr));

	memcpy (n_itr, itr, sizeof(struct oskiplist_itr));
	/* NOTE: this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool oskiplist_itr_equals(const_iterator a, const_iterator b) {
	const struct oskiplist_itr* itr_a = (const struct oskiplist_itr*)a;
	const struct oskiplist_itr* itr_b = (const struct oskiplist_itr*)b;

	dbg_assert(__is_object((unknown)a));
	dbg_assert(__is_object((unknown)b));
	dbg_assert(itr_a->__cast == oskiplist_itr_cast);
	dbg_assert(itr_b->__cast == oskiplist_itr_cast);

	return itr_a->__current == itr_b->__current;
}

static const void* oskiplist_itr_get_ref(const_iterator citr) {
	const struct oskiplist_itr* itr = (const struct oskiplist_itr*)citr;
	const struct skip_link* link    = itr->__current;

	dbg_assert(itr->__cast == oskiplist_itr_cast);
	dbg_assert(itr->__current != NULL);

	return skip_link_getref(link);
}

static void oskiplist_itr_set_ref(iterator citr, const void* n_ref) {
	/* skiplist does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(n_ref);

	dbg_assert(false);

	return;
}

static void oskiplist_itr_to_next(object* citr) {
	struct oskiplist_itr* itr    = (struct oskiplist_itr*)citr;

	dbg_assert(itr->__cast == oskiplist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = skip_link_next(itr->__current);
}

static void oskiplist_itr_to_prev(object* citr) {
	struct oskiplist_itr* itr = (struct oskiplist_itr*)citr;

	dbg_assert(itr->__cast == oskiplist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = skip_link_prev(itr->__current);
}

static unknown oskiplist_cast(unknown x, unique_id intf_id) {
	struct oskiplist* o = (struct oskiplist*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case ISET_ID:
		return (unknown)&o->__iftable[e_set];
		break;
	case IMSET_ID:
		return (unknown)&o->__iftable[e_mset];
		break;
	default:
		break;
	}

	return NULL;
}

static unknown oskiplist_itr_cast(unknown x, unique_id inf_id) {
	struct oskiplist_itr* itr = (struct oskiplist_itr*)x;

	dbg_assert(__is_object(itr));

	switch (inf_id) {
	case ITR_BAS_ID:
	case ITR_REF_ID:
	case ITR_ACC_ID:
	case ITR_FWD_ID:
	case ITR_BID_ID:
		return (unknown)&itr->__iftable[0];
	case ITR_RAC_ID:
		return NULL;
	default:
		return NULL;
	}

	return NULL;
}

static object* oskiplist_create(pf_compare ref_compare) {
	//return oskiplist_create_v(ref_compare, default_allocator, NULL);
	return oskiplist_create_v(ref_compare, NULL, NULL);
}

static void* skiplist_alloc_adapter(int size, void* context) {
	allocator alc = (allocator)context;

	return allocator_alloc(alc, size);
}

static bool skiplist_dealloc_adapter(void* buff, void* context) {
	allocator alc = (allocator)context;

	return allocator_dealloc(alc, buff);
}

static void oskiplist_itr_com_init(struct oskiplist_itr* itr, struct oskiplist* list);
static object* oskiplist_create_v(pf_compare ref_compare, allocator alc, pf_dispose dispose) {
	struct oskiplist* oskiplist = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 40);
		managed_allocator = true;
	}

	oskiplist = (struct oskiplist*)allocator_alloc(alc, sizeof(struct oskiplist));

	oskiplist->__offset = oskiplist;
	oskiplist->__cast   = oskiplist_cast;
	
	oskiplist->__iftable[e_set].__offset = (address)e_set;
	oskiplist->__iftable[e_set].__vtable = &__iset_vtable;
	oskiplist->__iftable[e_mset].__offset = (address)e_mset;
	oskiplist->__iftable[e_mset].__vtable = &__imset_vtable;

	oskiplist->__size      = 0;
	oskiplist->__ref_comp  = ref_compare;
	oskiplist->__skiplist  = skiplist_create_v(ref_compare, skiplist_alloc_adapter, skiplist_dealloc_adapter, alc);

	oskiplist->__allocator = alc;
	oskiplist->__allocator_join_ondispose = managed_allocator;

	oskiplist->__dispose   = dispose;

	/* initialize begin/end iterators, the position is reassigned when each query */
	oskiplist_itr_com_init(&oskiplist->__itr_begin, oskiplist);
	oskiplist_itr_com_init(&oskiplist->__itr_end, oskiplist);

	return (object*)oskiplist;
}

/* from ifactory.h  */
object* cntr_create_oskiplist(pf_compare comp) {
	return oskiplist_create(comp);
}

object* cntr_create_oskiplist_v(pf_compare comp, allocator alc, pf_dispose dispose) {
	return oskiplist_create_v(comp, alc, dispose);
}

static void oskiplist_destroy(object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	allocator alc = oskiplist->__allocator;
	bool join_alc = oskiplist->__allocator_join_ondispose;

	oskiplist_clear(o);
	skiplist_destroy(oskiplist->__skiplist);
	allocator_dealloc(alc, oskiplist);

	if (join_alc) {
		allocator_join(alc);
	}
}

void per_link_reference_destroy(const void* __ref, void* context) {
	pf_dispose ref_dispose = (pf_dispose)context;

	if (ref_dispose != NULL) {
		ref_dispose((void*)__ref);
	}
}

static void oskiplist_clear(object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;

	skiplist_foreach(oskiplist->__skiplist, per_link_reference_destroy, (void*)oskiplist->__dispose);
	skiplist_clear(oskiplist->__skiplist);

	oskiplist->__size = 0;
}

static int oskiplist_size(const object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;

	return oskiplist->__size;
}

static bool oskiplist_empty(const object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	return oskiplist->__size == 0;
}

/* initialize all part of an iterator except the __current position */
static void oskiplist_itr_com_init(struct oskiplist_itr* itr, struct oskiplist* list) {
	itr->__offset = itr;
	itr->__cast   = oskiplist_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__allocator = list->__allocator;
	/* itr->__current = NULL; */
}

static const_iterator oskiplist_itr_begin(const object* o) {
	struct oskiplist* olist = (struct oskiplist*)o;

	olist->__itr_begin.__current = skiplist_first(olist->__skiplist);

	return (iterator)&olist->__itr_begin;
}

static const_iterator oskiplist_itr_end(const object* o) {
	struct oskiplist* olist = (struct oskiplist*)o;

	olist->__itr_end.__current = skiplist_sent(olist->__skiplist);

	return (iterator)&olist->__itr_end;
}

static iterator oskiplist_itr_create(const object* o, itr_pos pos) {
	struct oskiplist* olist = (struct oskiplist*)o;
	struct oskiplist_itr* n_itr = (struct oskiplist_itr*)
		allocator_alloc(olist->__allocator, sizeof(struct oskiplist_itr));

	oskiplist_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->__current = skiplist_first(olist->__skiplist);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = skiplist_sent(olist->__skiplist);
	}
	
	return (object*)n_itr;
}

static void oskiplist_itr_assign(const object* o, iterator itr, itr_pos pos) {
	struct oskiplist* olist = (struct oskiplist*)o;
	struct oskiplist_itr* n_itr = (struct oskiplist_itr*)itr;

	oskiplist_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->__current = skiplist_first(olist->__skiplist);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = skiplist_sent(olist->__skiplist);
	}
}
static void oskiplist_itr_find(const object* o, iterator itr, void* __ref) {
	struct oskiplist* olist      = (struct oskiplist*)o;
	struct oskiplist_itr* oitr   = (struct oskiplist_itr*)itr;
	const struct skip_link* link = NULL;

	link = skiplist_search(olist->__skiplist, __ref);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

static void oskiplist_itr_find_lower(const object* o, iterator itr, void* __ref) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct oskiplist_itr* oitr  = (struct oskiplist_itr*)itr;
	const struct skip_link* link = NULL;

	link = skiplist_search_v(oskiplist->__skiplist, __ref, skiplist_min_greaterorequal);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

static void oskiplist_itr_find_upper(const object* o, iterator itr, void* __ref) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct oskiplist_itr* oitr  = (struct oskiplist_itr*)itr;
	const struct skip_link* link = NULL;

	link = skiplist_search_v(oskiplist->__skiplist, __ref, skiplist_min_greater);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

static bool oskiplist_insert_s(object* o, void* __ref) {
	struct oskiplist* oskiplist     = (struct oskiplist*)o;
	bool res = skiplist_insert_s(oskiplist->__skiplist, __ref);

	if (res == true)
		oskiplist->__size ++;

	return res;
}

static void oskiplist_insert_m(object* o, void* __ref) {
	struct oskiplist* oskiplist     = (struct oskiplist*)o;

	skiplist_insert(oskiplist->__skiplist, __ref);

	oskiplist->__size ++;
	return;
}

static bool oskiplist_contains(const object* o, void* __ref) {
	struct oskiplist* oskiplist   = (struct oskiplist*)o;

	return skiplist_contains(oskiplist->__skiplist, __ref);
}

static int oskiplist_count(const object* o, void* __ref) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	const struct skip_link* lb = skiplist_search_v(oskiplist->__skiplist, __ref, skiplist_min_greaterorequal);
	const struct skip_link* ub = skiplist_search_v(oskiplist->__skiplist, __ref, skiplist_min_greater);
	int count = 0;
	
	while (lb != ub) {
		count ++;
		lb = skip_link_next(lb);
	}

	return count;
}

static bool oskiplist_remove(object* o, void* __ref) {
	struct oskiplist* oskiplist   = (struct oskiplist*)o;

	bool res = skiplist_remove(oskiplist->__skiplist, __ref);

	if (res == true) 
		oskiplist->__size --;

	return res;
}

static void* oskiplist_itr_remove(object* o, iterator itr) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct oskiplist_itr* oitr  = (struct oskiplist_itr*)itr;
	struct skip_link* link      = (struct skip_link*)oitr->__current;
	const void* obj_ref         = skip_link_getref(link);

	dbg_assert(oitr->__cast == oskiplist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	skiplist_remove_link(oskiplist->__skiplist, link);

	oskiplist->__size --;

	return (void*)obj_ref;
}

