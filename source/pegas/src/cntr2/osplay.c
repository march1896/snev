#include <string.h>

#include <cntr2/iobject.h>
#include <cntr2/iset.h>
#include <cntr2/iitr.h>
#include <cntr2/ifactory.h>
#include <cntr2/osplay.h>
#include <cntr2/oallocator.h>

#include <util/splay_link.h>
#include <memheap/heap_global.h>

/* this module defines a left-lean red black tree container, which implements iset interface. */

enum splay_interfaces {
	e_object,
	e_set,
	e_mset,
	e_l_count
};

struct osplay_node {
	struct splaylink             link;

	unknown*                     reference;
};

struct osplay_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	struct base_interface         __iftable[itr_interface_count];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     allocator;
	struct osplay*                container;

	struct splaylink*             current;
};

/* binary search tree */
struct osplay {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	/* just a sentinel to represent the end of the tree, the maximum element of the tree */
	struct splaylink              sentinel;
	/* __root == __sentinel.left */
	struct splaylink*             root;

	int                           size;

	unknown_traits                content_traits;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;      

	struct osplay_itr             itr_begin;
	struct osplay_itr             itr_end;
};

static struct iobject_vtable __osplay_iobject_vtable = {
	splayset_destroy,          /* __destroy */
	splayset_clone,            /* __clone */
	splayset_equals,           /* __equals */
	splayset_compare_to,       /* __compare_to */
	splayset_hashcode,         /* __hashcode */
};

static struct iset_vtable __osplay_iset_vtable = {
	splayset_clear,            /* __clear */
	splayset_size,             /* __size */
	splayset_empty,            /* __empty */
	splayset_insert_s,         /* __insert */
	splayset_replace_s,        /* __replace */
	splayset_contains,         /* __contains */
	splayset_remove_s,         /* __remove */

	splayset_itr_create,       /* __itr_create */
	splayset_itr_assign,       /* __itr_assign */
	splayset_itr_find_s,       /* __itr_find */
	splayset_itr_remove,       /* __itr_remove */
	splayset_itr_begin,        /* __itr_begin */
	splayset_itr_end           /* __itr_end */
};

static struct imset_vtable __osplay_imset_vtable = {
	splayset_clear,            /* __clear */
	splayset_size,             /* __size */
	splayset_empty,            /* __empty */
	splayset_insert_m,         /* __insert */
	splayset_contains,         /* __contains */
	splayset_count_m,          /* __count */
	splayset_remove_m,         /* __remove */

	splayset_itr_create,       /* __itr_create */
	splayset_itr_assign,       /* __itr_assign */
	splayset_itr_find_lower_m, /* __itr_find_lower */
	splayset_itr_find_upper_m, /* __itr_find_upper */
	splayset_itr_remove,       /* __itr_remove */
	splayset_itr_begin,        /* __itr_begin */
	splayset_itr_end           /* __itr_end */
};

static unknown* osplay_itr_cast(unknown* x, unique_id inf_id);
static unknown* osplay_cast(unknown* x, unique_id intf_id);

static void osplay_itr_destroy(iterator citr);
static iterator osplay_itr_clone(const_iterator citr);
static bool osplay_itr_equals(const_iterator a, const_iterator b);
static int osplay_itr_compare_to(const_iterator itr, const_iterator other);
static hashcode osplay_itr_hashcode(const_iterator itr);
static unknown* osplay_itr_get_obj(const_iterator citr);
static void osplay_itr_set_obj(iterator citr, const unknown* n_ref);
static const unknown* osplay_itr_get_ref(const_iterator citr);
static void osplay_itr_swap_ref(iterator citr, iterator other);
static void osplay_itr_to_next(iterator citr);
static void osplay_itr_to_prev(iterator citr);

static struct iobject_vtable __osplay_itr_iobject_vtable = {
	osplay_itr_destroy,      /* __destroy */
	osplay_itr_clone,        /* __clone   */
	osplay_itr_equals,       /* __equals  */
	osplay_itr_compare_to,   /* __compare_to */
	osplay_itr_hashcode
};


static struct itr_bidirectional_vtable __osplay_itr_vtable = {
	osplay_itr_get_obj,      /* __get_obj */
	osplay_itr_get_ref,      /* __get_ref */
	osplay_itr_set_obj,      /* __set_obj */
	osplay_itr_swap_ref,     /* __swap_ref */
	osplay_itr_to_next,      /* __to_next */
	osplay_itr_to_prev       /* __to_prev */
};


static void osplay_itr_destroy(_object* citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == osplay_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->allocator, itr);
}

static iterator osplay_itr_clone(const_iterator citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;
	struct osplay_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == osplay_itr_cast);

	/* destroy itself */
	n_itr = (struct osplay_itr*)allocator_alloc(itr->allocator, sizeof(struct osplay_itr));

	memcpy (n_itr, itr, sizeof(struct osplay_itr));
	/* this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool osplay_itr_equals(const_iterator a, const_iterator b) {
	const struct osplay_itr* itr_a = (const struct osplay_itr*)a;
	const struct osplay_itr* itr_b = (const struct osplay_itr*)b;

	dbg_assert(__is_object((unknown*)a));
	dbg_assert(__is_object((unknown*)b));
	dbg_assert(itr_a->__cast == osplay_itr_cast);
	dbg_assert(itr_b->__cast == osplay_itr_cast);

	return itr_a->current == itr_b->current;
}

static int osplay_itr_compare_to(const_iterator itr, const_iterator other) {
	// TODO
	return 0;
}
static hashcode osplay_itr_hashcode(const_iterator itr) {
	// TODO
	return 0;
}

static unknown* osplay_itr_get_obj(const_iterator citr) {
	const struct osplay_itr* itr   = (const struct osplay_itr*)citr;
	const struct osplay_node* node = NULL;
	struct osplay* container = itr->container;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct osplay_node, link);

	//return node->reference;
	return container->content_traits.__clone(node->reference, (pf_alloc)__global_default_alloc, __global_default_heap);
}

static void osplay_itr_set_obj(iterator citr, const unknown* n_ref) {
	/* splay does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(n_ref);

	rt_error("osplay iterator is not accessible");

	return;
}

static const unknown* osplay_itr_get_ref(const_iterator citr) {
	const struct osplay_itr* itr   = (const struct osplay_itr*)citr;
	const struct osplay_node* node = NULL;
	//struct osplay* container = itr->container;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct osplay_node, link);

	return node->reference;
}

static void osplay_itr_swap_ref(iterator citr, iterator other) {
	/* splay does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(other);

	rt_error("osplay iterator is not accessible");

	return;
}

static void osplay_itr_to_next(_object* citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = splay_successor(itr->current, false);
}

static void osplay_itr_to_prev(_object* citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = splay_predesessor(itr->current, false);
}

static unknown* osplay_cast(unknown* x, unique_id intf_id) {
	struct osplay* o = (struct osplay*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IOBJECT_ID:
		return (unknown*)&o->__iftable[e_object];
		break;
	case ISET_ID:
		return (unknown*)&o->__iftable[e_set];
		break;
	case IMSET_ID:
		return (unknown*)&o->__iftable[e_mset];
		break;
	default:
		break;
	}

	return NULL;
}

static unknown* osplay_itr_cast(unknown* x, unique_id inf_id) {
	struct osplay_itr* itr = (struct osplay_itr*)x;

	dbg_assert(__is_object(itr));

	switch (inf_id) {
	case IOBJECT_ID:
		return (unknown*)&itr->__iftable[itr_interface_iobject];
	case ITR_REF_ID:
	case ITR_FWD_ID:
	case ITR_BID_ID:
		return (unknown*)&itr->__iftable[itr_interface_iterator];
	case ITR_ACC_ID:
		return NULL;
	case ITR_RAC_ID:
		return NULL;
	default:
		return NULL;
	}

	return NULL;
}

static compres osplay_splaylink_compare(const struct splaylink* a, const struct splaylink* b, void* param) {
	pf_ibridge_compare ref_comp = (pf_ibridge_compare)param;

	struct osplay_node* node_a = container_of(a, struct osplay_node, link);
	struct osplay_node* node_b = container_of(b, struct osplay_node, link);

	return ref_comp(node_a->reference, node_b->reference);
}

static void osplay_itr_com_init(struct osplay_itr* itr, struct osplay* list);
static _object* splayset_create_internal(unknown_traits* traits, allocator alc) {
	struct osplay* osplay = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	osplay = (struct osplay*)allocator_alloc(alc, sizeof(struct osplay));

	osplay->__offset = osplay;
	osplay->__cast   = osplay_cast;
	
	osplay->__iftable[e_object].__offset = (address)e_object;
	osplay->__iftable[e_object].__vtable = &__osplay_iobject_vtable;
	osplay->__iftable[e_set].__offset = (address)e_set;
	osplay->__iftable[e_set].__vtable = &__osplay_iset_vtable;
	osplay->__iftable[e_mset].__offset = (address)e_mset;
	osplay->__iftable[e_mset].__vtable = &__osplay_imset_vtable;

	osplay->size      = 0;

	// make sure the content is comparable.
	dbg_assert(traits->__compare_to != NULL);
	osplay->content_traits.__destroy    = traits->__destroy;
	osplay->content_traits.__clone      = traits->__clone;
	osplay->content_traits.__compare_to = traits->__compare_to;
	osplay->content_traits.__equals     = traits->__equals;
	osplay->content_traits.__hashcode   = traits->__hashcode;

	osplay->root      = NULL;
	osplay->sentinel.left   = NULL;
	osplay->sentinel.right  = NULL;
	osplay->sentinel.parent = NULL;

	osplay->allocator = alc;
	osplay->allocator_join_ondispose = managed_allocator;

	/* initialize begin/end iterators, the position is reassigned when each query */
	osplay_itr_com_init(&osplay->itr_begin, osplay);
	osplay_itr_com_init(&osplay->itr_end, osplay);

	return (_object*)osplay;
}

_object* splayset_create(unknown_traits* content_traits, allocator alc) {
	return splayset_create_internal(content_traits, alc);
}

/* from ifactory.h  */
_object* cntr_create_osplay(unknown_traits* content_traits) {
	return splayset_create_internal(content_traits, __global_default_allocator);
}
_object* cntr_create_osplay_a(unknown_traits* content_traits, allocator alc) {
	return splayset_create_internal(content_traits, alc);
}

void splayset_destroy(_object* o) {
	struct osplay* osplay = (struct osplay*)o;
	allocator alc = osplay->allocator;
	bool join_alc = osplay->allocator_join_ondispose;

	splayset_clear(o);

	allocator_dealloc(alc, osplay);

	if (join_alc) {
		allocator_join(alc);
	}
}

_object* splayset_clone(const _object* o) {
	// TODO
	return NULL;
}
bool splayset_equals(const _object* o, const _object* other) {
	// TODO
	return false;
}
int splayset_compare_to(const _object* o, const _object* other) {
	// TODO
	return 1;
}
hashcode splayset_hashcode(const _object* o) {
	// TODO
	return (hashcode)NULL;
}

typedef void (*pf_per_link_operation)(struct splaylink* link, void* param);
static void splay_traverse(struct splaylink* cur, pf_per_link_operation cb, void* param) {
	if (cur == NULL) return;

	splay_traverse(cur->left, cb, param);
	splay_traverse(cur->right, cb, param);
	cb(cur, param);
}

static inline void osplay_detach_sentinel(struct osplay* osplay) {
	if (osplay->root != NULL)
		osplay->root->parent = NULL;

	osplay->sentinel.left = NULL;
}

/* we have to re-associate the sentinel and the root node after we change the tree */
static inline void osplay_attach_sentinel(struct osplay* osplay) {
	osplay->sentinel.left = osplay->root;

	if (osplay->root != NULL)
		osplay->root->parent = &osplay->sentinel;
}

static void splaylink_dispose(struct splaylink* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct osplay* osplay = (struct osplay*)param;

	osplay->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, osplay->allocator);

	/* delete the node it self */
	allocator_dealloc(osplay->allocator, node);
}
void splayset_clear(_object* o) {
	struct osplay* osplay = (struct osplay*)o;

	splay_traverse(osplay->root, splaylink_dispose, (void*)osplay);

	osplay->root = NULL;
	osplay_attach_sentinel(osplay);
	osplay->size = 0;
}

int splayset_size(const _object* o) {
	struct osplay* osplay = (struct osplay*)o;

	return osplay->size;
}

bool splayset_empty(const _object* o) {
	struct osplay* osplay = (struct osplay*)o;
	return osplay->size == 0;
}

/* initialize all part of an iterator except the __current position */
static void osplay_itr_com_init(struct osplay_itr* itr, struct osplay* list) {
	itr->__offset = itr;
	itr->__cast   = osplay_itr_cast;

	itr->__iftable[itr_interface_iobject].__offset = (address)itr_interface_iobject;
	itr->__iftable[itr_interface_iobject].__vtable = (unknown*)&__osplay_itr_iobject_vtable;
	itr->__iftable[itr_interface_iterator].__offset = (address)itr_interface_iterator;
	itr->__iftable[itr_interface_iterator].__vtable = (unknown*)&__osplay_itr_vtable;

	itr->allocator = list->allocator;
	itr->container = list;
	/* itr->__current = NULL; */
}

const_iterator splayset_itr_begin(const _object* o) {
	struct osplay* osplay = (struct osplay*)o;

	osplay->itr_begin.current = splay_min(osplay->root);

	return (iterator)&osplay->itr_begin;
}

const_iterator splayset_itr_end(const _object* o) {
	struct osplay* osplay = (struct osplay*)o;

	osplay->itr_end.current = &osplay->sentinel;

	return (iterator)&osplay->itr_end;
}

iterator splayset_itr_create(const _object* o, itr_pos pos) {
	struct osplay* osplay = (struct osplay*)o;
	struct osplay_itr* n_itr = (struct osplay_itr*)
		allocator_alloc(osplay->allocator, sizeof(struct osplay_itr));

	osplay_itr_com_init(n_itr, osplay);

	if (pos == itr_begin) {
		if (osplay->root == NULL) {
			n_itr->current = &osplay->sentinel;
		} else {
			n_itr->current = splay_min(osplay->root);
		}
	}
	else if (pos == itr_end) {
		n_itr->current = &osplay->sentinel;
	}
	else {
		dbg_assert(false);
	}
	
	return (_object*)n_itr;
}

void splayset_itr_assign(const _object* o, iterator itr, itr_pos pos) {
	struct osplay* osplay = (struct osplay*)o;
	struct osplay_itr* n_itr = (struct osplay_itr*)itr;

	osplay_itr_com_init(n_itr, osplay);

	if (pos == itr_begin) {
		if (osplay->root == NULL) {
			n_itr->current = &osplay->sentinel;
		} else {
			n_itr->current = splay_min(osplay->root);
		}
	}
	else if (pos == itr_end){
		/* __sentinel will be the maximum element in the tree */
		n_itr->current = &osplay->sentinel;
	}
	else {
		dbg_assert(false);
	}
}

struct direct_s {
	pf_ibridge_compare  comp;
	const unknown*   target;
	const struct splaylink* candidate; /* only useful for multiple instances */
};

static int osplay_direct(const struct splaylink* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = 0;

	compr = dir->comp(node->reference, dir->target);

	if (compr == 0)
		return 0;
	else if (compr < 0) /* we should expose right side */
		return 1;
	else 
		return -1;
}

static int osplay_direct_lower(const struct splaylink* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = 0;

	compr = dir->comp(node->reference, dir->target);

	if (compr == 0) {
		dir->candidate = link; /* update the candidate */
		return -1; /* explore the left side */
	}
	else if (compr < 0) /* we should explore right side */
		return 1;
	else {
		dir->candidate = link;
		return -1;
	}

	/* never been here */
	return 0;
}

static int osplay_direct_upper(const struct splaylink* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = 0;

	compr = dir->comp(node->reference, dir->target);

	if (compr == 0) {
		return 1; /* explore the right side */
	}
	else if (compr < 0) /* we should explore right side */
		return 1;
	else {
		dir->candidate = link; /* update the candidate */
		return -1;
	}

	/* never been here */
	return 0;
}

void splayset_itr_find_s(const _object* o, iterator itr, const unknown* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct direct_s   dir    = { osplay->content_traits.__compare_to, __ref, NULL };
	struct splaylink* find_res = NULL; 
	
	osplay_detach_sentinel(osplay);
	find_res = splay_dynamic_search(&osplay->root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	dbg_assert(dir.candidate == NULL);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (unknown*)&__osplay_itr_vtable);

	if (find_res != NULL) {
		oitr->current = find_res;
	}
	else {
		oitr->current = &osplay->sentinel;
	}
}

void splayset_itr_find_lower_m(const _object* o, iterator itr, const unknown* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct direct_s   dir    = { osplay->content_traits.__compare_to, __ref, NULL };
	struct splaylink* link  = NULL;

	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->root, osplay_direct_lower, &dir);
	osplay_attach_sentinel(osplay);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct splaylink*)dir.candidate;    /* the last candidate, the most closed to leaf one, is what we want */

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (unknown*)&__osplay_itr_vtable);

	if (link != NULL) {
		oitr->current = link;
	}
	else {
		oitr->current = &osplay->sentinel;
	}
}

void splayset_itr_find_upper_m(const _object* o, iterator itr, const unknown* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct direct_s   dir    = { osplay->content_traits.__compare_to, __ref, NULL };
	struct splaylink* link  = NULL;
	
	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->root, osplay_direct_upper, &dir);
	osplay_attach_sentinel(osplay);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct splaylink*)dir.candidate;

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (unknown*)&__osplay_itr_vtable);

	if (link != NULL) { 
		oitr->current = link;
	}
	else {
		oitr->current = &osplay->sentinel;
	}
}

void splayset_insert_s(_object* o, const unknown* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_node* node = (struct osplay_node*)
		allocator_alloc(osplay->allocator, sizeof(struct osplay_node));
	struct splaylink* duplicated = NULL;

	/* just assign the reference, delay the clone operation if there is no duplicates */
	node->reference = (unknown*)__ref;

	osplay_detach_sentinel(osplay);
	osplay->root = splay_insert_sv(osplay->root, &node->link, osplay_splaylink_compare, osplay->content_traits.__compare_to, &duplicated);
	osplay_attach_sentinel(osplay);

	if (duplicated != NULL) {
		dbg_assert(osplay_splaylink_compare(&node->link, duplicated, osplay->content_traits.__compare_to) == 0);
		
		allocator_dealloc(osplay->allocator, node);
	} else {
		osplay->size ++;
		node->reference = osplay->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, osplay->allocator);
	}
}

void splayset_replace_s(_object* o, const unknown* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_node* node = (struct osplay_node*)
		allocator_alloc(osplay->allocator, sizeof(struct osplay_node));
	struct splaylink* duplicated = NULL;

	node->reference = osplay->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, osplay->allocator);

	osplay_detach_sentinel(osplay);
	osplay->root = splay_insert_sv(osplay->root, &node->link, osplay_splaylink_compare, osplay->content_traits.__compare_to, &duplicated);
	osplay_attach_sentinel(osplay);

	if (duplicated != NULL) {
		struct osplay_node* dup_node = container_of(duplicated, struct osplay_node, link);
		unknown* old_ref = dup_node->reference;

		dbg_assert(osplay_splaylink_compare(&node->link, duplicated, osplay->content_traits.__compare_to) == 0);

		/* assign the duplicate node a new reference */
		dup_node->reference = node->reference;

		allocator_dealloc(osplay->allocator, node);

		osplay->content_traits.__destroy(old_ref, (pf_dealloc)allocator_release, osplay->allocator);;
	} else {
		osplay->size ++;
	}
}

void splayset_insert_m(_object* o, const unknown* __ref) {
	struct osplay* osplay     = (struct osplay*)o;
	struct osplay_node* node = (struct osplay_node*)
		allocator_alloc(osplay->allocator, sizeof(struct osplay_node));

	node->reference = osplay->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, osplay->allocator);

	osplay_detach_sentinel(osplay);
	osplay->root = splay_insert_v(osplay->root, &node->link, osplay_splaylink_compare, osplay->content_traits.__compare_to);
	osplay_attach_sentinel(osplay);

	osplay->size ++;
	return;
}

bool splayset_contains(const _object* o, const unknown* __ref) {
	struct osplay* osplay   = (struct osplay*)o;
	struct direct_s   dir   = { osplay->content_traits.__compare_to, __ref, NULL };
	struct splaylink* link = NULL;
	
	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	if (link != NULL) {
		return true;
	}
	
	return false;
}

int splayset_count_m(const _object* o, const unknown* __ref) {
	struct osplay*     osplay   = (struct osplay*)o;
	struct direct_s   dir       = { osplay->content_traits.__compare_to, __ref, NULL };
	const struct splaylink* lb = NULL;
	
	osplay_detach_sentinel(osplay);
	lb = splay_dynamic_search(&osplay->root, osplay_direct_lower, &dir);
	dbg_assert(lb == NULL);
	osplay_attach_sentinel(osplay);

	lb = (dir.candidate);
	dir.candidate = NULL;
	if (lb != NULL) {
		const struct splaylink* ub = splay_search(osplay->root, osplay_direct_upper, &dir);
		int count = 0;

		dbg_assert(ub == NULL);
		ub = (dir.candidate);
		
		if (ub == NULL) 
			ub = &osplay->sentinel;

		while (lb != ub) {
			count ++;
			lb = splay_successor(lb, false);
		}

		return count;
	}

	return 0;
}

bool splayset_remove_s(_object* o, const unknown* __ref) {
	struct osplay* osplay   = (struct osplay*)o;
	struct direct_s   dir   = { osplay->content_traits.__compare_to, __ref, NULL };
	struct splaylink* link = NULL;
	
	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	if (link != NULL) {
		struct osplay_node* node = container_of(link, struct osplay_node, link);

		osplay_detach_sentinel(osplay);
		osplay->root = splay_remove_v(osplay->root, link, osplay_splaylink_compare, osplay->content_traits.__compare_to);
		osplay_attach_sentinel(osplay);

		osplay->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, osplay->allocator);

		allocator_dealloc(osplay->allocator, node);

		osplay->size --;

		return true;
	}

	return false;
}

int splayset_remove_m(_object* o, const unknown* __ref) {
	struct osplay* osplay   = (struct osplay*)o;
	struct direct_s   dir   = { osplay->content_traits.__compare_to, __ref, NULL };
	struct splaylink* link = NULL;
	int count = 0;

	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	while (link != NULL) {
		struct osplay_node* node = container_of(link, struct osplay_node, link);

		osplay_detach_sentinel(osplay);
		osplay->root = splay_remove_v(osplay->root, link, osplay_splaylink_compare, osplay->content_traits.__compare_to);
		osplay_attach_sentinel(osplay);

		osplay->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, osplay->allocator);

		allocator_dealloc(osplay->allocator, node);

		osplay->size --;
		count ++;

		osplay_detach_sentinel(osplay);
		link = splay_dynamic_search(&osplay->root, osplay_direct, &dir);
		osplay_attach_sentinel(osplay);
	}

	return count;
}


void splayset_itr_remove(_object* o, iterator itr) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct osplay_node* node = container_of(oitr->current, struct osplay_node, link);

	dbg_assert(oitr->__cast == osplay_itr_cast);
	dbg_assert(oitr->current != NULL);

	osplay_detach_sentinel(osplay);
	osplay->root = splay_remove_v(osplay->root, &node->link, osplay_splaylink_compare, osplay->content_traits.__compare_to);
	osplay_attach_sentinel(osplay);

	osplay->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, osplay->allocator);
	allocator_dealloc(osplay->allocator, node);

	/* invalidate the iterator */
	oitr->current = NULL;

	osplay->size --;
}

