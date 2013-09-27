#include <cntr2/imap.h>
#include <cntr2/iitr.h>

#include <cntr2/ifactory.h>
#include <cntr2/oallocator.h>

#include <cntr2/ollrb.h>
#include <cntr2/oskiplist.h>
#include <cntr2/osplay.h>

/* this module implements ordered map, by order set */
enum map_interfaces {
	e_map,
	e_mmap,
	e_mi_count
};

struct oordmap_itr {
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
struct oordmap {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	iobject*                      __driver_set;

	int                           __size;
	pf_compare                    __ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     __allocator;
	bool                          __allocator_join_ondispose;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_dispose                    __dispose;         

	struct oordmap_itr            __itr_begin;
	struct oordmap_itr            __itr_end;
};

static struct iset_vtable __iset_vtable = {
	oordmap_destroy,          /* __destroy */
	oordmap_clear,            /* __clear */
	oordmap_size,             /* __size */
	oordmap_empty,            /* __empty */
	oordmap_insert_s,         /* __insert */
	oordmap_contains,         /* __contains */
	oordmap_remove,           /* __remove */

	oordmap_itr_create,       /* __itr_create */
	oordmap_itr_assign,       /* __itr_assign */
	oordmap_itr_find,         /* __itr_find */
	oordmap_itr_remove,       /* __itr_remove */
	oordmap_itr_begin,        /* __itr_begin */
	oordmap_itr_end           /* __itr_end */
};

static struct imset_vtable __imset_vtable = {
	oordmap_destroy,          /* __destroy */
	oordmap_clear,            /* __clear */
	oordmap_size,             /* __size */
	oordmap_empty,            /* __empty */
	oordmap_insert_m,         /* __insert */
	oordmap_contains,         /* __contains */
	oordmap_count,            /* __count */
	oordmap_remove,           /* __remove */

	oordmap_itr_create,       /* __itr_create */
	oordmap_itr_assign,       /* __itr_assign */
	oordmap_itr_find_lower,   /* __itr_find_lower */
	oordmap_itr_find_upper,   /* __itr_find_upper */
	oordmap_itr_remove,       /* __itr_remove */
	oordmap_itr_begin,        /* __itr_begin */
	oordmap_itr_end           /* __itr_end */
};

static unknown oordmap_itr_cast(unknown x, unique_id inf_id);
static unknown oordmap_cast(unknown x, unique_id intf_id);

static void oordmap_itr_destroy(iterator citr);
static iterator oordmap_itr_clone(const_iterator citr);
static bool oordmap_itr_equals(const_iterator a, const_iterator b);
static const void* oordmap_itr_get_ref(const_iterator citr);
static void oordmap_itr_set_ref(iterator citr, const void* n_ref);
static void oordmap_itr_to_next(iterator citr);
static void oordmap_itr_to_prev(iterator citr);

static struct itr_bidirectional_vtable __itr_vtable = {
	oordmap_itr_destroy,      /* __destroy */
	oordmap_itr_clone,        /* __clone   */
	oordmap_itr_equals,       /* __equals  */
	oordmap_itr_get_ref,      /* __get_ref */
	oordmap_itr_set_ref,      /* __set_ref */
	oordmap_itr_to_next,      /* __to_next */
	oordmap_itr_to_prev       /* __to_prev */
};


static void oordmap_itr_destroy(object* citr) {
	struct oordmap_itr* itr = (struct oordmap_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oordmap_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->__allocator, itr);
}

static iterator oordmap_itr_clone(const_iterator citr) {
	struct oordmap_itr* itr = (struct oordmap_itr*)citr;
	struct oordmap_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oordmap_itr_cast);

	/* destroy itself */
	n_itr = (struct oordmap_itr*)allocator_alloc(itr->__allocator, sizeof(struct oordmap_itr));

	memcpy (n_itr, itr, sizeof(struct oordmap_itr));
	/* NOTE: this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool oordmap_itr_equals(const_iterator a, const_iterator b) {
	const struct oordmap_itr* itr_a = (const struct oordmap_itr*)a;
	const struct oordmap_itr* itr_b = (const struct oordmap_itr*)b;

	dbg_assert(__is_object((unknown)a));
	dbg_assert(__is_object((unknown)b));
	dbg_assert(itr_a->__cast == oordmap_itr_cast);
	dbg_assert(itr_b->__cast == oordmap_itr_cast);

	return itr_a->__current == itr_b->__current;
}

static const void* oordmap_itr_get_ref(const_iterator citr) {
	const struct oordmap_itr* itr = (const struct oordmap_itr*)citr;
	const struct skip_link* link    = itr->__current;

	dbg_assert(itr->__cast == oordmap_itr_cast);
	dbg_assert(itr->__current != NULL);

	return skip_link_getref(link);
}

static void oordmap_itr_set_ref(iterator citr, const void* n_ref) {
	/* ordmap does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(n_ref);

	dbg_assert(false);

	return;
}

static void oordmap_itr_to_next(object* citr) {
	struct oordmap_itr* itr    = (struct oordmap_itr*)citr;

	dbg_assert(itr->__cast == oordmap_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = skip_link_next(itr->__current);
}

static void oordmap_itr_to_prev(object* citr) {
	struct oordmap_itr* itr = (struct oordmap_itr*)citr;

	dbg_assert(itr->__cast == oordmap_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = skip_link_prev(itr->__current);
}

static unknown oordmap_cast(unknown x, unique_id intf_id) {
	struct oordmap* o = (struct oordmap*)x;

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

static unknown oordmap_itr_cast(unknown x, unique_id inf_id) {
	struct oordmap_itr* itr = (struct oordmap_itr*)x;

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

object* oordmap_create(pf_compare ref_compare) {
	return oordmap_create_v(ref_compare, __global_default_allocator, NULL);
}

static void* ordmap_alloc_adapter(int size, void* context) {
	allocator alc = (allocator)context;

	return allocator_alloc(alc, size);
}

static bool ordmap_dealloc_adapter(void* buff, void* context) {
	allocator alc = (allocator)context;

	return allocator_dealloc(alc, buff);
}

static void oordmap_itr_com_init(struct oordmap_itr* itr, struct oordmap* list);
object* oordmap_create_v(pf_compare ref_compare, allocator alc, pf_dispose dispose) {
	struct oordmap* oordmap = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 40);
		managed_allocator = true;
	}

	oordmap = (struct oordmap*)allocator_alloc(alc, sizeof(struct oordmap));

	oordmap->__offset = oordmap;
	oordmap->__cast   = oordmap_cast;
	
	oordmap->__iftable[e_set].__offset = (address)e_set;
	oordmap->__iftable[e_set].__vtable = &__iset_vtable;
	oordmap->__iftable[e_mset].__offset = (address)e_mset;
	oordmap->__iftable[e_mset].__vtable = &__imset_vtable;

	oordmap->__size      = 0;
	oordmap->__ref_comp  = ref_compare;
	oordmap->__ordmap  = ordmap_create_v(ref_compare, ordmap_alloc_adapter, ordmap_dealloc_adapter, alc);

	oordmap->__allocator = alc;
	oordmap->__allocator_join_ondispose = managed_allocator;

	oordmap->__dispose   = dispose;

	/* initialize begin/end iterators, the position is reassigned when each query */
	oordmap_itr_com_init(&oordmap->__itr_begin, oordmap);
	oordmap_itr_com_init(&oordmap->__itr_end, oordmap);

	return (object*)oordmap;
}

/* from ifactory.h  */
object* cntr_create_oordmap(pf_compare comp) {
	return oordmap_create(comp);
}

object* cntr_create_oordmap_v(pf_compare comp, allocator alc, pf_dispose dispose) {
	return oordmap_create_v(comp, alc, dispose);
}

void oordmap_destroy(object* o) {
	struct oordmap* oordmap = (struct oordmap*)o;
	allocator alc = oordmap->__allocator;
	bool join_alc = oordmap->__allocator_join_ondispose;

	oordmap_clear(o);
	ordmap_destroy(oordmap->__ordmap);
	allocator_dealloc(alc, oordmap);

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

void oordmap_clear(object* o) {
	struct oordmap* oordmap = (struct oordmap*)o;

	ordmap_foreach(oordmap->__ordmap, per_link_reference_destroy, (void*)oordmap->__dispose);
	ordmap_clear(oordmap->__ordmap);

	oordmap->__size = 0;
}

int oordmap_size(const object* o) {
	struct oordmap* oordmap = (struct oordmap*)o;

	return oordmap->__size;
}

bool oordmap_empty(const object* o) {
	struct oordmap* oordmap = (struct oordmap*)o;
	return oordmap->__size == 0;
}

/* initialize all part of an iterator except the __current position */
static void oordmap_itr_com_init(struct oordmap_itr* itr, struct oordmap* list) {
	itr->__offset = itr;
	itr->__cast   = oordmap_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__allocator = list->__allocator;
	/* itr->__current = NULL; */
}

const_iterator oordmap_itr_begin(const object* o) {
	struct oordmap* olist = (struct oordmap*)o;

	olist->__itr_begin.__current = ordmap_first(olist->__ordmap);

	return (iterator)&olist->__itr_begin;
}

const_iterator oordmap_itr_end(const object* o) {
	struct oordmap* olist = (struct oordmap*)o;

	olist->__itr_end.__current = ordmap_sent(olist->__ordmap);

	return (iterator)&olist->__itr_end;
}

iterator oordmap_itr_create(const object* o, itr_pos pos) {
	struct oordmap* olist = (struct oordmap*)o;
	struct oordmap_itr* n_itr = (struct oordmap_itr*)
		allocator_alloc(olist->__allocator, sizeof(struct oordmap_itr));

	oordmap_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->__current = ordmap_first(olist->__ordmap);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = ordmap_sent(olist->__ordmap);
	}
	
	return (object*)n_itr;
}

void oordmap_itr_assign(const object* o, iterator itr, itr_pos pos) {
	struct oordmap* olist = (struct oordmap*)o;
	struct oordmap_itr* n_itr = (struct oordmap_itr*)itr;

	oordmap_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->__current = ordmap_first(olist->__ordmap);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = ordmap_sent(olist->__ordmap);
	}
}
void oordmap_itr_find(const object* o, iterator itr, const void* __ref) {
	struct oordmap* olist      = (struct oordmap*)o;
	struct oordmap_itr* oitr   = (struct oordmap_itr*)itr;
	const struct skip_link* link = NULL;

	link = ordmap_search(olist->__ordmap, __ref);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

void oordmap_itr_find_lower(const object* o, iterator itr, const void* __ref) {
	struct oordmap* oordmap = (struct oordmap*)o;
	struct oordmap_itr* oitr  = (struct oordmap_itr*)itr;
	const struct skip_link* link = NULL;

	link = ordmap_search_v(oordmap->__ordmap, __ref, ordmap_min_greaterorequal);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

void oordmap_itr_find_upper(const object* o, iterator itr, const void* __ref) {
	struct oordmap* oordmap = (struct oordmap*)o;
	struct oordmap_itr* oitr  = (struct oordmap_itr*)itr;
	const struct skip_link* link = NULL;

	link = ordmap_search_v(oordmap->__ordmap, __ref, ordmap_min_greater);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

bool oordmap_insert_s(object* o, const void* __ref) {
	struct oordmap* oordmap     = (struct oordmap*)o;
	bool res = ordmap_insert_s(oordmap->__ordmap, __ref);

	if (res == true)
		oordmap->__size ++;

	return res;
}

void oordmap_insert_m(object* o, const void* __ref) {
	struct oordmap* oordmap     = (struct oordmap*)o;

	ordmap_insert(oordmap->__ordmap, __ref);

	oordmap->__size ++;
	return;
}

bool oordmap_contains(const object* o, const void* __ref) {
	struct oordmap* oordmap   = (struct oordmap*)o;

	return ordmap_contains(oordmap->__ordmap, __ref);
}

int oordmap_count(const object* o, const void* __ref) {
	struct oordmap* oordmap = (struct oordmap*)o;
	const struct skip_link* lb = ordmap_search_v(oordmap->__ordmap, __ref, ordmap_min_greaterorequal);
	const struct skip_link* ub = ordmap_search_v(oordmap->__ordmap, __ref, ordmap_min_greater);
	int count = 0;
	
	while (lb != ub) {
		count ++;
		lb = skip_link_next(lb);
	}

	return count;
}

bool oordmap_remove(object* o, void* __ref) {
	struct oordmap* oordmap   = (struct oordmap*)o;

	bool res = ordmap_remove(oordmap->__ordmap, __ref);

	if (res == true) 
		oordmap->__size --;

	return res;
}

void* oordmap_itr_remove(object* o, iterator itr) {
	struct oordmap* oordmap = (struct oordmap*)o;
	struct oordmap_itr* oitr  = (struct oordmap_itr*)itr;
	struct skip_link* link      = (struct skip_link*)oitr->__current;
	const void* obj_ref         = skip_link_getref(link);

	dbg_assert(oitr->__cast == oordmap_itr_cast);
	dbg_assert(oitr->__current != NULL);

	ordmap_remove_link(oordmap->__ordmap, link);

	oordmap->__size --;

	return (void*)obj_ref;
}

