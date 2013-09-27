#include <string.h>

#include <cntr2/iset.h>
#include <cntr2/iitr.h>
#include <cntr2/ifactory.h>
#include <cntr2/oskiplist.h>
#include <cntr2/oallocator.h>

#include <util/skiplist.h>

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
	allocator                     allocator;

	const struct skip_link*       current;
};

/* binary search tree */
struct oskiplist {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	struct skiplist*              driver_skiplist;

	int                           size;
	pf_ref_compare                ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;      

	struct oskiplist_itr          itr_begin;
	struct oskiplist_itr          itr_end;
};

static struct iset_vtable __iset_vtable = {
	oskiplist_destroy,          /* __destroy */
	oskiplist_clear,            /* __clear */
	oskiplist_clear_v,          /* __clear_v */
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
	oskiplist_clear_v,          /* __clear_v */
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
	allocator_dealloc(itr->allocator, itr);
}

static iterator oskiplist_itr_clone(const_iterator citr) {
	struct oskiplist_itr* itr = (struct oskiplist_itr*)citr;
	struct oskiplist_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oskiplist_itr_cast);

	/* destroy itself */
	n_itr = (struct oskiplist_itr*)allocator_alloc(itr->allocator, sizeof(struct oskiplist_itr));

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

	return itr_a->current == itr_b->current;
}

static const void* oskiplist_itr_get_ref(const_iterator citr) {
	const struct oskiplist_itr* itr = (const struct oskiplist_itr*)citr;
	const struct skip_link* link    = itr->current;

	dbg_assert(itr->__cast == oskiplist_itr_cast);
	dbg_assert(itr->current != NULL);

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
	dbg_assert(itr->current != NULL);

	itr->current = skip_link_next(itr->current);
}

static void oskiplist_itr_to_prev(object* citr) {
	struct oskiplist_itr* itr = (struct oskiplist_itr*)citr;

	dbg_assert(itr->__cast == oskiplist_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = skip_link_prev(itr->current);
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

object* oskiplist_create(pf_ref_compare ref_compare) {
	return oskiplist_create_v(ref_compare, __global_default_allocator);
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
object* oskiplist_create_v(pf_ref_compare ref_compare, allocator alc) {
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

	oskiplist->size      = 0;
	oskiplist->ref_comp  = ref_compare;
	oskiplist->driver_skiplist  = skiplist_create_v(ref_compare, skiplist_alloc_adapter, skiplist_dealloc_adapter, alc);

	oskiplist->allocator = alc;
	oskiplist->allocator_join_ondispose = managed_allocator;

	/* initialize begin/end iterators, the position is reassigned when each query */
	oskiplist_itr_com_init(&oskiplist->itr_begin, oskiplist);
	oskiplist_itr_com_init(&oskiplist->itr_end, oskiplist);

	return (object*)oskiplist;
}

/* from ifactory.h  */
object* cntr_create_oskiplist(pf_ref_compare comp) {
	return oskiplist_create(comp);
}

object* cntr_create_oskiplist_v(pf_ref_compare comp, allocator alc) {
	return oskiplist_create_v(comp, alc);
}

void oskiplist_destroy(object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	allocator alc = oskiplist->allocator;
	bool join_alc = oskiplist->allocator_join_ondispose;

	oskiplist_clear(o);
	skiplist_destroy(oskiplist->driver_skiplist);
	allocator_dealloc(alc, oskiplist);

	if (join_alc) {
		allocator_join(alc);
	}
}

struct skiplist_clear_v {
	pf_ref_dispose_v function;
	void*            context;
};

void oskiplist_clear(object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;

	skiplist_clear(oskiplist->driver_skiplist);

	oskiplist->size = 0;
}

static void skiplist_perlink_dispose(const void* __ref, void* context) {
	struct skiplist_clear_v* rd = (struct skiplist_clear_v*)context;

	if (rd->function != NULL) {
		rd->function((void*)__ref, rd->context);
	}
}

void oskiplist_clear_v(object* o, pf_ref_dispose_v dispose, void* context) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct skiplist_clear_v rd = { dispose, context };

	if (dispose != NULL) {
		skiplist_foreach(oskiplist->driver_skiplist, (pf_ref_visit_v)skiplist_perlink_dispose, (void*)&rd);
	}
	
	skiplist_clear(oskiplist->driver_skiplist);

	oskiplist->size = 0;
}

int oskiplist_size(const object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;

	return oskiplist->size;
}

bool oskiplist_empty(const object* o) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	return oskiplist->size == 0;
}

/* initialize all part of an iterator except the __current position */
static void oskiplist_itr_com_init(struct oskiplist_itr* itr, struct oskiplist* list) {
	itr->__offset = itr;
	itr->__cast   = oskiplist_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->allocator = list->allocator;
	/* itr->__current = NULL; */
}

const_iterator oskiplist_itr_begin(const object* o) {
	struct oskiplist* olist = (struct oskiplist*)o;

	olist->itr_begin.current = skiplist_first(olist->driver_skiplist);

	return (iterator)&olist->itr_begin;
}

const_iterator oskiplist_itr_end(const object* o) {
	struct oskiplist* olist = (struct oskiplist*)o;

	olist->itr_end.current = skiplist_sent(olist->driver_skiplist);

	return (iterator)&olist->itr_end;
}

iterator oskiplist_itr_create(const object* o, itr_pos pos) {
	struct oskiplist* olist = (struct oskiplist*)o;
	struct oskiplist_itr* n_itr = (struct oskiplist_itr*)
		allocator_alloc(olist->allocator, sizeof(struct oskiplist_itr));

	oskiplist_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->current = skiplist_first(olist->driver_skiplist);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->current = skiplist_sent(olist->driver_skiplist);
	}
	
	return (object*)n_itr;
}

void oskiplist_itr_assign(const object* o, iterator itr, itr_pos pos) {
	struct oskiplist* olist = (struct oskiplist*)o;
	struct oskiplist_itr* n_itr = (struct oskiplist_itr*)itr;

	oskiplist_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->current = skiplist_first(olist->driver_skiplist);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->current = skiplist_sent(olist->driver_skiplist);
	}
}
void oskiplist_itr_find(const object* o, iterator itr, const void* __ref) {
	struct oskiplist* olist      = (struct oskiplist*)o;
	struct oskiplist_itr* oitr   = (struct oskiplist_itr*)itr;
	const struct skip_link* link = NULL;

	link = skiplist_search(olist->driver_skiplist, __ref);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->current = link;
}

void oskiplist_itr_find_lower(const object* o, iterator itr, const void* __ref) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct oskiplist_itr* oitr  = (struct oskiplist_itr*)itr;
	const struct skip_link* link = NULL;

	link = skiplist_search_v(oskiplist->driver_skiplist, __ref, skiplist_min_greaterorequal);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->current = link;
}

void oskiplist_itr_find_upper(const object* o, iterator itr, const void* __ref) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct oskiplist_itr* oitr  = (struct oskiplist_itr*)itr;
	const struct skip_link* link = NULL;

	link = skiplist_search_v(oskiplist->driver_skiplist, __ref, skiplist_min_greater);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->current = link;
}

void* oskiplist_insert_s(object* o, const void* __ref) {
	struct oskiplist* oskiplist     = (struct oskiplist*)o;
	void* old_ref = skiplist_insert_s(oskiplist->driver_skiplist, __ref);

	if (old_ref == NULL)
		oskiplist->size ++;

	return old_ref;
}

void oskiplist_insert_m(object* o, const void* __ref) {
	struct oskiplist* oskiplist     = (struct oskiplist*)o;

	skiplist_insert(oskiplist->driver_skiplist, __ref);

	oskiplist->size ++;
	return;
}

bool oskiplist_contains(const object* o, const void* __ref) {
	struct oskiplist* oskiplist   = (struct oskiplist*)o;

	return skiplist_contains(oskiplist->driver_skiplist, __ref);
}

int oskiplist_count(const object* o, const void* __ref) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	const struct skip_link* lb = skiplist_search_v(oskiplist->driver_skiplist, __ref, skiplist_min_greaterorequal);
	const struct skip_link* ub = skiplist_search_v(oskiplist->driver_skiplist, __ref, skiplist_min_greater);
	int count = 0;
	
	while (lb != ub) {
		count ++;
		lb = skip_link_next(lb);
	}

	return count;
}

bool oskiplist_remove(object* o, void* __ref) {
	struct oskiplist* oskiplist   = (struct oskiplist*)o;

	bool res = skiplist_remove(oskiplist->driver_skiplist, __ref);

	if (res == true) 
		oskiplist->size --;

	return res;
}

void* oskiplist_itr_remove(object* o, iterator itr) {
	struct oskiplist* oskiplist = (struct oskiplist*)o;
	struct oskiplist_itr* oitr  = (struct oskiplist_itr*)itr;
	struct skip_link* link      = (struct skip_link*)oitr->current;
	const void* obj_ref         = skip_link_getref(link);

	dbg_assert(oitr->__cast == oskiplist_itr_cast);
	dbg_assert(oitr->current != NULL);

	skiplist_remove_link(oskiplist->driver_skiplist, link);

	oskiplist->size --;

	return (void*)obj_ref;
}

