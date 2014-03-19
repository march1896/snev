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

struct ordmap_itr {
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
struct ordmap {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_mi_count];

	int                           size;
	pf_ref_compare                ref_comp;

	iobject*                      driven_set;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;

	struct ordmap_itr             itr_begin;
	struct ordmap_itr             itr_end;
};

static struct imap_vtable __imap_vtable = {
	ordmap_destroy,          /* __destroy */
	ordmap_clear,            /* __clear */
	ordmap_clear_v,          /* __clear_v */
	ordmap_size,             /* __size */
	ordmap_empty,            /* __empty */
	ordmap_insert_s,         /* __insert */
	ordmap_contains,         /* __contains */
	ordmap_findbykey,        /* __findbykey */
	ordmap_remove,           /* __remove */

	ordmap_itr_create,       /* __itr_create */
	ordmap_itr_assign,       /* __itr_assign */
	ordmap_itr_find,         /* __itr_find */
	ordmap_itr_remove,       /* __itr_remove */
	ordmap_itr_begin,        /* __itr_begin */
	ordmap_itr_end           /* __itr_end */
};

static struct immap_vtable __immap_vtable = {
	ordmap_destroy,          /* __destroy */
	ordmap_clear,            /* __clear */
	ordmap_clear_v,          /* __clear_v */
	ordmap_size,             /* __size */
	ordmap_empty,            /* __empty */
	ordmap_insert_m,         /* __insert */
	ordmap_contains,         /* __contains */
	ordmap_count,            /* __count */
	ordmap_remove,           /* __remove */

	ordmap_itr_create,       /* __itr_create */
	ordmap_itr_assign,       /* __itr_assign */
	ordmap_itr_find_lower,   /* __itr_find_lower */
	ordmap_itr_find_upper,   /* __itr_find_upper */
	ordmap_itr_remove,       /* __itr_remove */
	ordmap_itr_begin,        /* __itr_begin */
	ordmap_itr_end           /* __itr_end */
};

static unknown ordmap_itr_cast(unknown x, unique_id inf_id);
static unknown ordmap_cast(unknown x, unique_id intf_id);

static void     ordmap_itr_destroy(iterator citr);
static iterator ordmap_itr_clone(const_iterator citr);
static bool     ordmap_itr_equals(const_iterator a, const_iterator b);
static const void* ordmap_itr_get_ref(const_iterator citr);
static void     ordmap_itr_set_ref(iterator citr, const void* n_ref);
static void     ordmap_itr_to_next(iterator citr);
static void     ordmap_itr_to_prev(iterator citr);

static struct itr_bidirectional_vtable __itr_vtable = {
	ordmap_itr_destroy,      /* __destroy */
	ordmap_itr_clone,        /* __clone   */
	ordmap_itr_equals,       /* __equals  */
	ordmap_itr_get_ref,      /* __get_ref */
	ordmap_itr_set_ref,      /* __set_ref */
	ordmap_itr_to_next,      /* __to_next */
	ordmap_itr_to_prev       /* __to_prev */
};


static void ordmap_itr_destroy(object* citr) {
	struct ordmap_itr* itr = (struct ordmap_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == ordmap_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->__allocator, itr);
}

static iterator ordmap_itr_clone(const_iterator citr) {
	struct ordmap_itr* itr   = (struct ordmap_itr*)citr;
	struct ordmap_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == ordmap_itr_cast);

	/* destroy itself */
	n_itr = (struct ordmap_itr*)allocator_alloc(itr->__allocator, sizeof(struct ordmap_itr));

	memcpy (n_itr, itr, sizeof(struct ordmap_itr));
	/* NOTE: this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool ordmap_itr_equals(const_iterator a, const_iterator b) {
	const struct ordmap_itr* itr_a = (const struct ordmap_itr*)a;
	const struct ordmap_itr* itr_b = (const struct ordmap_itr*)b;

	dbg_assert(__is_object((unknown)a));
	dbg_assert(__is_object((unknown)b));
	dbg_assert(itr_a->__cast == ordmap_itr_cast);
	dbg_assert(itr_b->__cast == ordmap_itr_cast);

	return itr_a->__current == itr_b->__current;
}

static const void* ordmap_itr_get_ref(const_iterator citr) {
	const struct ordmap_itr* itr = (const struct ordmap_itr*)citr;
	const struct skip_link* link    = itr->__current;

	dbg_assert(itr->__cast == ordmap_itr_cast);
	dbg_assert(itr->__current != NULL);

	return skip_link_getref(link);
}

static void ordmap_itr_set_ref(iterator citr, const void* n_ref) {
	/* ordmap does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(n_ref);

	dbg_assert(false);

	return;
}

static void ordmap_itr_to_next(object* citr) {
	struct ordmap_itr* itr    = (struct ordmap_itr*)citr;

	dbg_assert(itr->__cast == ordmap_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = skip_link_next(itr->__current);
}

static void ordmap_itr_to_prev(object* citr) {
	struct ordmap_itr* itr = (struct ordmap_itr*)citr;

	dbg_assert(itr->__cast == ordmap_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = skip_link_prev(itr->__current);
}

static unknown ordmap_cast(unknown x, unique_id intf_id) {
	struct ordmap* o = (struct ordmap*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IMAP_ID:
		return (unknown)&o->__iftable[e_map];
		break;
	case IMMAP_ID:
		return (unknown)&o->__iftable[e_mmap];
		break;
	default:
		break;
	}

	return NULL;
}

static unknown ordmap_itr_cast(unknown x, unique_id inf_id) {
	struct ordmap_itr* itr = (struct ordmap_itr*)x;

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

object* ordmap_create(pf_compare ref_compare) {
	return ordmap_create_v(ref_compare, __global_default_allocator, NULL);
}

static void* ordmap_alloc_adapter(int size, void* context) {
	allocator alc = (allocator)context;

	return allocator_alloc(alc, size);
}

static bool ordmap_dealloc_adapter(void* buff, void* context) {
	allocator alc = (allocator)context;

	return allocator_dealloc(alc, buff);
}

static int map_pair_compare(const void* ref_a, const void* ref_b) {

}

static void ordmap_itr_com_init(struct ordmap_itr* itr, struct ordmap* list);
object* ordmap_create_v(pf_ref_compare key_compare, allocator alc, enum ordmap_driver driver_type) {
	struct ordmap* ordmap = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 40);
		managed_allocator = true;
	}

	ordmap = (struct ordmap*)allocator_alloc(alc, sizeof(struct ordmap));

	ordmap->__offset = ordmap;
	ordmap->__cast   = ordmap_cast;
	
	ordmap->__iftable[e_set].__offset = (address)e_set;
	ordmap->__iftable[e_set].__vtable = &__iset_vtable;
	ordmap->__iftable[e_mset].__offset = (address)e_mset;
	ordmap->__iftable[e_mset].__vtable = &__imset_vtable;

	ordmap->size      = 0;
	ordmap->ref_comp  = ref_compare;

	switch (driver_type) {
		case ordmap_by_llrb:
			break;
		case ordmap_by_splay:
			break;
		case ordmap_by_skiplist:
			break;
		default:
			break;
	}
	ordmap->driven_set = cntr_create_ollrb( 
	ordmap->__ordmap   = ordmap_create_v(ref_compare, allocator_require, allocator_release, alc);

	ordmap->allocator = alc;
	ordmap->allocator_join_ondispose = managed_allocator;

	/* initialize begin/end iterators, the position is reassigned when each query */
	ordmap_itr_com_init(&ordmap->itr_begin, ordmap);
	ordmap_itr_com_init(&ordmap->itr_end, ordmap);

	return (object*)ordmap;
}

/* from ifactory.h  */
object* cntr_create_ordmap(pf_compare comp) {
	return ordmap_create(comp);
}

object* cntr_create_ordmap_v(pf_compare comp, allocator alc, pf_dispose dispose) {
	return ordmap_create_v(comp, alc, dispose);
}

void ordmap_destroy(object* o) {
	struct ordmap* ordmap = (struct ordmap*)o;
	allocator alc = ordmap->allocator;
	bool join_alc = ordmap->allocator_join_ondispose;

	iset_destroy(ordmap->driven_set);
	ordmap_clear(o);
	ordmap_destroy(ordmap->__ordmap);
	allocator_dealloc(alc, ordmap);

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

void ordmap_clear(object* o) {
	struct ordmap* ordmap = (struct ordmap*)o;

	ordmap_foreach(ordmap->__ordmap, per_link_reference_destroy, (void*)ordmap->dispose);
	ordmap_clear(ordmap->__ordmap);

	ordmap->size = 0;
}

int ordmap_size(const object* o) {
	struct ordmap* ordmap = (struct ordmap*)o;

	return ordmap->size;
}

bool ordmap_empty(const object* o) {
	struct ordmap* ordmap = (struct ordmap*)o;
	return ordmap->size == 0;
}

/* initialize all part of an iterator except the __current position */
static void ordmap_itr_com_init(struct ordmap_itr* itr, struct ordmap* list) {
	itr->__offset = itr;
	itr->__cast   = ordmap_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__allocator = list->allocator;
	/* itr->__current = NULL; */
}

const_iterator ordmap_itr_begin(const object* o) {
	struct ordmap* olist = (struct ordmap*)o;

	olist->itr_begin.__current = ordmap_first(olist->__ordmap);

	return (iterator)&olist->itr_begin;
}

const_iterator ordmap_itr_end(const object* o) {
	struct ordmap* olist = (struct ordmap*)o;

	olist->itr_end.__current = ordmap_sent(olist->__ordmap);

	return (iterator)&olist->itr_end;
}

iterator ordmap_itr_create(const object* o, itr_pos pos) {
	struct ordmap* olist = (struct ordmap*)o;
	struct ordmap_itr* n_itr = (struct ordmap_itr*)
		allocator_alloc(olist->allocator, sizeof(struct ordmap_itr));

	ordmap_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->__current = ordmap_first(olist->__ordmap);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = ordmap_sent(olist->__ordmap);
	}
	
	return (object*)n_itr;
}

void ordmap_itr_assign(const object* o, iterator itr, itr_pos pos) {
	struct ordmap* olist = (struct ordmap*)o;
	struct ordmap_itr* n_itr = (struct ordmap_itr*)itr;

	ordmap_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		n_itr->__current = ordmap_first(olist->__ordmap);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = ordmap_sent(olist->__ordmap);
	}
}
void ordmap_itr_find(const object* o, iterator itr, const void* __ref) {
	struct ordmap* olist      = (struct ordmap*)o;
	struct ordmap_itr* oitr   = (struct ordmap_itr*)itr;
	const struct skip_link* link = NULL;

	link = ordmap_search(olist->__ordmap, __ref);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

void ordmap_itr_find_lower(const object* o, iterator itr, const void* __ref) {
	struct ordmap* ordmap = (struct ordmap*)o;
	struct ordmap_itr* oitr  = (struct ordmap_itr*)itr;
	const struct skip_link* link = NULL;

	link = ordmap_search_v(ordmap->__ordmap, __ref, ordmap_min_greaterorequal);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

void ordmap_itr_find_upper(const object* o, iterator itr, const void* __ref) {
	struct ordmap* ordmap = (struct ordmap*)o;
	struct ordmap_itr* oitr  = (struct ordmap_itr*)itr;
	const struct skip_link* link = NULL;

	link = ordmap_search_v(ordmap->__ordmap, __ref, ordmap_min_greater);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	oitr->__current = link;
}

bool ordmap_insert_s(object* o, const void* __ref) {
	struct ordmap* ordmap     = (struct ordmap*)o;
	bool res = ordmap_insert_s(ordmap->__ordmap, __ref);

	if (res == true)
		ordmap->size ++;

	return res;
}

void ordmap_insert_m(object* o, const void* __ref) {
	struct ordmap* ordmap     = (struct ordmap*)o;

	ordmap_insert(ordmap->__ordmap, __ref);

	ordmap->size ++;
	return;
}

bool ordmap_contains(const object* o, const void* __ref) {
	struct ordmap* ordmap   = (struct ordmap*)o;

	return ordmap_contains(ordmap->__ordmap, __ref);
}

int ordmap_count(const object* o, const void* __ref) {
	struct ordmap* ordmap = (struct ordmap*)o;
	const struct skip_link* lb = ordmap_search_v(ordmap->__ordmap, __ref, ordmap_min_greaterorequal);
	const struct skip_link* ub = ordmap_search_v(ordmap->__ordmap, __ref, ordmap_min_greater);
	int count = 0;
	
	while (lb != ub) {
		count ++;
		lb = skip_link_next(lb);
	}

	return count;
}

bool ordmap_remove(object* o, void* __ref) {
	struct ordmap* ordmap   = (struct ordmap*)o;

	bool res = ordmap_remove(ordmap->__ordmap, __ref);

	if (res == true) 
		ordmap->size --;

	return res;
}

void* ordmap_itr_remove(object* o, iterator itr) {
	struct ordmap* ordmap = (struct ordmap*)o;
	struct ordmap_itr* oitr  = (struct ordmap_itr*)itr;
	struct skip_link* link      = (struct skip_link*)oitr->__current;
	const void* obj_ref         = skip_link_getref(link);

	dbg_assert(oitr->__cast == ordmap_itr_cast);
	dbg_assert(oitr->__current != NULL);

	ordmap_remove_link(ordmap->__ordmap, link);

	ordmap->size --;

	return (void*)obj_ref;
}

