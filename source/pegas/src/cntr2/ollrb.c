#include <string.h>

#include <cntr2/iobject.h>
#include <cntr2/iset.h>
#include <cntr2/iitr.h>
#include <cntr2/ifactory.h>
#include <cntr2/ollrb.h>
#include <cntr2/oallocator.h>

#include <util/llrb_link.h>
#include <memheap/heap_global.h>

/* this module defines a left-lean red black tree container, which implements iset interface. */

enum llrb_interfaces {
	e_object,
	e_set,
	e_mset,
	e_l_count
};

struct ollrb_node {
	struct llrblink              link;

	unknown*                     reference;
};


struct ollrb_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	struct base_interface         __iftable[itr_interface_count];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     allocator;

	struct ollrb*                 container;

	struct llrblink*              current;
};

/* binary search tree */
struct ollrb {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	/* just a sentinel to represent the end of the tree, the maximum element of the tree */
	struct llrblink               sentinel;
	/* __root == __sentinel.left */
	struct llrblink*              root;

	int                           size;

	unknown_traits                content_traits;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;      

	struct ollrb_itr              itr_begin;
	struct ollrb_itr              itr_end;
};

static struct iobject_vtable __ollrb_iobject_vtable = {
	ollrb_destroy,          /* __destroy */
	ollrb_clone,            /* __clone */
	ollrb_equals,           /* __equals */
	ollrb_compare_to,       /* __compare_to */
	ollrb_hashcode,         /* __hashcode */
};

static struct iset_vtable __ollrb_iset_vtable = {
	ollrb_clear,            /* __clear */
	ollrb_size,             /* __size */
	ollrb_empty,            /* __empty */
	ollrb_insert_s,         /* __insert */
	ollrb_replace_s,        /* __replace */
	ollrb_contains,         /* __contains */
	ollrb_remove_s,         /* __remove */

	ollrb_itr_create,       /* __itr_create */
	ollrb_itr_assign,       /* __itr_assign */
	ollrb_itr_find,         /* __itr_find */
	ollrb_itr_remove,       /* __itr_remove */
	ollrb_itr_begin,        /* __itr_begin */
	ollrb_itr_end           /* __itr_end */
};

static struct imset_vtable __ollrb_imset_vtable = {
	ollrb_clear,            /* __clear */
	ollrb_size,             /* __size */
	ollrb_empty,            /* __empty */
	ollrb_insert_m,         /* __insert */
	ollrb_contains,         /* __contains */
	ollrb_count,            /* __count */
	ollrb_remove_m,         /* __remove */

	ollrb_itr_create,       /* __itr_create */
	ollrb_itr_assign,       /* __itr_assign */
	ollrb_itr_find_lower,   /* __itr_find_lower */
	ollrb_itr_find_upper,   /* __itr_find_upper */
	ollrb_itr_remove,       /* __itr_remove */
	ollrb_itr_begin,        /* __itr_begin */
	ollrb_itr_end           /* __itr_end */
};

static unknown* ollrb_itr_cast(unknown* x, unique_id inf_id);
static unknown* ollrb_cast(unknown* x, unique_id intf_id);

static void ollrb_itr_destroy(iterator citr);
static iterator ollrb_itr_clone(const_iterator citr);
static bool ollrb_itr_equals(const_iterator itr, const_iterator other);
static int ollrb_itr_compare_to(const_iterator itr, const_iterator other);
static hashcode ollrb_itr_hashcode(const_iterator itr);
static unknown* ollrb_itr_get_obj(const_iterator citr);
static void ollrb_itr_set_obj(iterator citr, const unknown* n_ref);
static const unknown* ollrb_itr_get_ref(const_iterator citr);
static void ollrb_itr_swap_ref(iterator citr, iterator other);
static void ollrb_itr_to_next(iterator citr);
static void ollrb_itr_to_prev(iterator citr);

static struct iobject_vtable __ollrb_itr_iobject_vtable = {
	ollrb_itr_destroy,      /* __destroy */
	ollrb_itr_clone,        /* __clone   */
	ollrb_itr_equals,       /* __equals  */
	ollrb_itr_compare_to,   /* __compare_to */
	ollrb_itr_hashcode
};

static struct itr_bidirectional_vtable __ollrb_itr_vtable = {
	ollrb_itr_get_obj,      /* __get_obj */
	ollrb_itr_get_ref,      /* __get_ref */
	ollrb_itr_set_obj,      /* __set_obj */
	ollrb_itr_swap_ref,     /* __swap_ref */
	ollrb_itr_to_next,      /* __to_next */
	ollrb_itr_to_prev       /* __to_prev */
};


static void ollrb_itr_destroy(_object* citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == ollrb_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->allocator, itr);
}

static iterator ollrb_itr_clone(const_iterator citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;
	struct ollrb_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == ollrb_itr_cast);

	/* destroy itself */
	n_itr = (struct ollrb_itr*)allocator_alloc(itr->allocator, sizeof(struct ollrb_itr));

	memcpy (n_itr, itr, sizeof(struct ollrb_itr));
	/* this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool ollrb_itr_equals(const_iterator a, const_iterator b) {
	const struct ollrb_itr* itr_a = (const struct ollrb_itr*)a;
	const struct ollrb_itr* itr_b = (const struct ollrb_itr*)b;

	dbg_assert(__is_object((unknown*)a));
	dbg_assert(__is_object((unknown*)b));
	dbg_assert(itr_a->__cast == ollrb_itr_cast);
	dbg_assert(itr_b->__cast == ollrb_itr_cast);
	dbg_assert(itr_a->container == itr_b->container);

	return itr_a->current == itr_b->current;
}

int ollrb_itr_compare_to(const_iterator a, const_iterator b) {
	// TODO
	dbg_assert(false);
	return 0;
}

hashcode ollrb_itr_hashcode(const_iterator itr) {
	// TODO
	dbg_assert(false);
	return 0;
}

static unknown* ollrb_itr_get_obj(const_iterator citr) {
	const struct ollrb_itr* itr   = (const struct ollrb_itr*)citr;
	const struct ollrb_node* node = NULL;
	struct ollrb* container = itr->container;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct ollrb_node, link);

	return container->content_traits.__clone(node->reference, (pf_alloc)__global_default_alloc, __global_default_heap);
}

static void ollrb_itr_set_obj(iterator citr, const unknown* n_ref) {
	/* llrb does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(n_ref);

	rt_error("ollrb iterator is not accessible");

	return;
}

static const unknown* ollrb_itr_get_ref(const_iterator citr) {
	const struct ollrb_itr* itr   = (const struct ollrb_itr*)citr;
	const struct ollrb_node* node = NULL;
	//struct ollrb* container = itr->container;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct ollrb_node, link);

	return node->reference;
}

static void ollrb_itr_swap_ref(iterator citr, iterator other) {
	/* llrb does not permit to set ref, which would destroy the inner data structure. */
	unused(citr);
	unused(other);

	rt_error("ollrb iterator is not accessible");

	return;
}

static void ollrb_itr_to_next(_object* citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = llrb_successor(itr->current, false);
}

static void ollrb_itr_to_prev(_object* citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = llrb_predesessor(itr->current, false);
}

static unknown* ollrb_cast(unknown* x, unique_id intf_id) {
	struct ollrb* o = (struct ollrb*)x;

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

static unknown* ollrb_itr_cast(unknown* x, unique_id inf_id) {
	struct ollrb_itr* itr = (struct ollrb_itr*)x;

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

/* the llrb compare context is a pf_ref_compare function pointer */
static compres ollrb_llrblink_compare(const struct llrblink* a, const struct llrblink* b, void* param) {
	pf_ibridge_compare ref_comp = (pf_ibridge_compare)param;

	struct ollrb_node* node_a = container_of(a, struct ollrb_node, link);
	struct ollrb_node* node_b = container_of(b, struct ollrb_node, link);

	return ref_comp(node_a->reference, node_b->reference);
}

static void ollrb_itr_com_init(struct ollrb_itr* itr, struct ollrb* list);
static _object* ollrb_create_internal(unknown_traits* traits, allocator alc) {
	struct ollrb* ollrb = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	ollrb = (struct ollrb*)allocator_alloc(alc, sizeof(struct ollrb));

	ollrb->__offset = ollrb;
	ollrb->__cast   = ollrb_cast;
	
	ollrb->__iftable[e_object].__offset = (address)e_object;
	ollrb->__iftable[e_object].__vtable = &__ollrb_iobject_vtable;
	ollrb->__iftable[e_set].__offset = (address)e_set;
	ollrb->__iftable[e_set].__vtable = &__ollrb_iset_vtable;
	ollrb->__iftable[e_mset].__offset = (address)e_mset;
	ollrb->__iftable[e_mset].__vtable = &__ollrb_imset_vtable;

	ollrb->size            = 0;
	dbg_assert(traits->__compare_to != NULL);
	ollrb->content_traits.__destroy    = traits->__destroy;
	ollrb->content_traits.__clone      = traits->__clone;
	ollrb->content_traits.__compare_to = traits->__compare_to;
	ollrb->content_traits.__equals     = traits->__equals;
	ollrb->content_traits.__hashcode   = traits->__hashcode;

	ollrb->root            = NULL;
	ollrb->sentinel.left   = NULL;
	ollrb->sentinel.right  = NULL;
	ollrb->sentinel.parent = NULL;
	ollrb->sentinel.color  = 55; /* Just a magic color, cause we will never reference it */


	ollrb->allocator = alc;
	ollrb->allocator_join_ondispose = managed_allocator;

	/* initialize begin/end iterators, the position is reassigned when each query */
	ollrb_itr_com_init(&ollrb->itr_begin, ollrb);
	ollrb_itr_com_init(&ollrb->itr_end, ollrb);

	return (_object*)ollrb;
}

_object* ollrb_create(unknown_traits* content_traits, allocator alc) {
	return ollrb_create_internal(content_traits, alc);
}

/* from ifactory.h  */
_object* cntr_create_ollrb(unknown_traits* content_traits) {
	return ollrb_create_internal(content_traits, __global_default_allocator);
}
_object* cntr_create_ollrb_a(unknown_traits* content_traits, allocator alc) {
	return ollrb_create_internal(content_traits, alc);
}

void ollrb_destroy(_object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;
	allocator alc = ollrb->allocator;
	bool join_alc = ollrb->allocator_join_ondispose;

	ollrb_clear(o);

	allocator_dealloc(alc, ollrb);

	if (join_alc) {
		allocator_join(alc);
	}
}

_object* ollrb_clone(const _object* o) {
	// TODO
	return NULL;
}
bool ollrb_equals(const _object* o, const _object* other) {
	// TODO
	return false;
}
int ollrb_compare_to(const _object* o, const _object* other) {
	// TODO
	return 1;
}
hashcode ollrb_hashcode(const _object* o) {
	// TODO
	return (hashcode)NULL;
}

typedef void (*pf_per_link_operation)(struct llrblink* link, void* param);
static void llrb_traverse(struct llrblink* cur, pf_per_link_operation cb, void* param) {
	if (cur == NULL) return;

	llrb_traverse(cur->left, cb, param);
	llrb_traverse(cur->right, cb, param);
	cb(cur, param);
}

/* we have to re-associate the sentinel and the root node after we change the tree */
static inline void ollrb_reassociate(struct ollrb* ollrb) {
	ollrb->sentinel.left = ollrb->root;

	if (ollrb->root != NULL)
		ollrb->root->parent = &ollrb->sentinel;
}

static void ollrblink_dispose(struct llrblink* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
	struct ollrb* ollrb     = (struct ollrb*)param;

	ollrb->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, ollrb->allocator);

	/* TODO: this is error prone, since if the memory manager reset the memory after dealloc, the tree link 
	   structure will be destroyed */
	allocator_dealloc(ollrb->allocator, node);
}

void ollrb_clear(_object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	llrb_traverse(ollrb->root, ollrblink_dispose, (void*)ollrb);

	ollrb->root = NULL;
	ollrb_reassociate(ollrb);
	ollrb->size = 0;
}

int ollrb_size(const _object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	return ollrb->size;
}

bool ollrb_empty(const _object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;
	return ollrb->size == 0;
}

/* initialize all part of an iterator except the __current position */
static void ollrb_itr_com_init(struct ollrb_itr* itr, struct ollrb* list) {
	itr->__offset = itr;
	itr->__cast   = ollrb_itr_cast;

	itr->__iftable[itr_interface_iobject].__offset = (address)itr_interface_iobject;
	itr->__iftable[itr_interface_iobject].__vtable = (unknown*)&__ollrb_itr_iobject_vtable;
	itr->__iftable[itr_interface_iterator].__offset = (address)itr_interface_iterator;
	itr->__iftable[itr_interface_iterator].__vtable = (unknown*)&__ollrb_itr_vtable;

	itr->allocator = list->allocator;
	itr->container = list;
	/* itr->__current = NULL; */
}

const_iterator ollrb_itr_begin(const _object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb->itr_begin.current = llrb_min(ollrb->root);

	return (iterator)&ollrb->itr_begin;
}

const_iterator ollrb_itr_end(const _object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb->itr_end.current = &ollrb->sentinel;

	return (iterator)&ollrb->itr_end;
}

iterator ollrb_itr_create(const _object* o, itr_pos pos) {
	struct ollrb* ollrb = (struct ollrb*)o;
	struct ollrb_itr* n_itr = (struct ollrb_itr*)
		allocator_alloc(ollrb->allocator, sizeof(struct ollrb_itr));

	ollrb_itr_com_init(n_itr, ollrb);

	if (pos == itr_begin) {
		if (ollrb->root == NULL) {
			n_itr->current = &ollrb->sentinel;
		} else {
			n_itr->current = llrb_min(ollrb->root);
		}
	}
	else if (pos == itr_end) {
		/* __sentinel will be the maximum element in the tree */
		n_itr->current = &ollrb->sentinel;
	}
	else {
		dbg_assert(false);
	}
	
	return (_object*)n_itr;
}

void ollrb_itr_assign(const _object* o, iterator itr, itr_pos pos) {
	struct ollrb* ollrb = (struct ollrb*)o;
	struct ollrb_itr* n_itr = (struct ollrb_itr*)itr;

	ollrb_itr_com_init(n_itr, ollrb);

	if (pos == itr_begin) {
		if (ollrb->root == NULL) {
			n_itr->current = &ollrb->sentinel;
		} else {
			n_itr->current = llrb_min(ollrb->root);
		}
	}
	else if (pos == itr_end) {
		/* __sentinel will be the maximum element in the tree */
		n_itr->current = &ollrb->sentinel;
	}
	else {
		dbg_assert(false);
	}
}

struct direct_s {
	pf_ibridge_compare   comp;
	const unknown*    target;
	const struct llrblink* candidate; /* only useful for multiple instances */
};

static int ollrb_direct(const struct llrblink* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
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

static int ollrb_direct_lower(const struct llrblink* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
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

static int ollrb_direct_upper(const struct llrblink* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
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

void ollrb_itr_find(const _object* o, iterator itr, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->content_traits.__compare_to, __ref, NULL };
	struct llrblink* link  = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);

	dbg_assert(dir.candidate == NULL);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (unknown*)&__ollrb_itr_vtable);

	if (link != NULL) {
		oitr->current = link;
	}
	else {
		oitr->current = &ollrb->sentinel;
	}
}

void ollrb_itr_find_lower(const _object* o, iterator itr, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->content_traits.__compare_to, __ref, NULL };
	struct llrblink* link  = llrb_search(ollrb->sentinel.left, ollrb_direct_lower, &dir);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct llrblink*)dir.candidate;    /* the last candidate, the most closed to leaf one, is what we want */

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (unknown*)&__ollrb_itr_vtable);

	if (link != NULL) {
		oitr->current = link;
	}
	else {
		oitr->current = &ollrb->sentinel;
	}
}

void ollrb_itr_find_upper(const _object* o, iterator itr, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->content_traits.__compare_to, __ref, NULL };
	struct llrblink* link  = llrb_search(ollrb->sentinel.left, ollrb_direct_upper, &dir);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct llrblink*)dir.candidate;

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (unknown*)&__ollrb_itr_vtable);

	if (link != NULL) { 
		oitr->current = link;
	}
	else {
		oitr->current = &ollrb->sentinel;
	}
}

void ollrb_insert_s(_object* o, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->allocator, sizeof(struct ollrb_node));
	struct llrblink* duplicated = NULL;

	/* just assign the reference, delay the clone operation if there is no duplicates */
	node->reference = (unknown*)__ref;

	ollrb->root = llrb_insert_sv(ollrb->root, &node->link, ollrb_llrblink_compare, ollrb->content_traits.__compare_to, &duplicated);
	ollrb_reassociate(ollrb);

	if (duplicated != NULL) {
		/* we can not insert the now reference */
		dbg_assert(ollrb_llrblink_compare(&node->link, duplicated, ollrb->content_traits.__compare_to) == 0);

		allocator_dealloc(ollrb->allocator, node);
	} else {
		node->reference = ollrb->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, ollrb->allocator);
		ollrb->size ++;
	}
}

void ollrb_replace_s(_object* o, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->allocator, sizeof(struct ollrb_node));
	struct llrblink* duplicated = NULL;

	/* just assign the reference, delay the clone operation if there is no duplicates */
	node->reference = ollrb->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, ollrb->allocator);

	ollrb->root = llrb_insert_sv(ollrb->root, &node->link, ollrb_llrblink_compare, ollrb->content_traits.__compare_to, &duplicated);
	ollrb_reassociate(ollrb);

	if (duplicated != NULL) {
		/* we can not insert the now reference, replace the old reference to new one */
		struct ollrb_node* dup_node = container_of(duplicated, struct ollrb_node, link);
		unknown* old_ref = dup_node->reference;

		dbg_assert(ollrb_llrblink_compare(&node->link, duplicated, ollrb->content_traits.__compare_to) == 0);

		/* assign the duplicate node a new reference */
		dup_node->reference = node->reference;

		allocator_dealloc(ollrb->allocator, node);

		ollrb->content_traits.__destroy(old_ref, (pf_dealloc)allocator_release, ollrb->allocator);;
	} else {
		ollrb->size ++;
	}
}

void ollrb_insert_m(_object* o, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->allocator, sizeof(struct ollrb_node));

	node->reference = ollrb->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, ollrb->allocator);

	ollrb->root = llrb_insert_v(ollrb->root, &node->link, ollrb_llrblink_compare, ollrb->content_traits.__compare_to);
	ollrb_reassociate(ollrb);

	ollrb->size ++;
}

bool ollrb_contains(const _object* o, const unknown* __ref) {
	struct ollrb* ollrb    = (struct ollrb*)o;
	struct direct_s   dir  = { ollrb->content_traits.__compare_to, __ref, NULL };
	struct llrblink* link = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);

	if (link != NULL) {
		return true;
	}
	
	return false;
}

int ollrb_count(const _object* o, const unknown* __ref) {
	struct ollrb*     ollrb    = (struct ollrb*)o;
	struct direct_s   dir      = { ollrb->content_traits.__compare_to, __ref, NULL };
	const struct llrblink* lb = llrb_search(ollrb->sentinel.left, ollrb_direct_lower, &dir);
	dbg_assert(lb == NULL);
	lb = (dir.candidate);

	dir.candidate = NULL;
	if (lb != NULL) {
		const struct llrblink* ub = llrb_search(ollrb->sentinel.left, ollrb_direct_upper, &dir);
		int count = 0;

		dbg_assert(ub == NULL);
		ub = (dir.candidate);
		
		if (ub == NULL) 
			ub = &ollrb->sentinel;

		while (lb != ub) {
			count ++;
			lb = llrb_successor(lb, false);
		}

		return count;
	}

	return 0;
}

bool ollrb_remove_s(_object* o, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct direct_s   dir   = { ollrb->content_traits.__compare_to, __ref, NULL };
	struct llrblink* link   = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);

	if (link != NULL) {
		struct ollrb_node* node = container_of(link, struct ollrb_node, link);

		/* first remove the link from the tree, no memory change */
		ollrb->root = llrb_remove_v(ollrb->root, link, ollrb_llrblink_compare, ollrb->content_traits.__compare_to);
		ollrb_reassociate(ollrb);

		/* second destroy the reference */
		ollrb->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, ollrb->allocator);

		/* third dealloc the node */
		allocator_dealloc(ollrb->allocator, node);

		ollrb->size --;

		return true;
	}

	return false;
}

int ollrb_remove_m(_object* o, const unknown* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct direct_s   dir   = { ollrb->content_traits.__compare_to, __ref, NULL };
	struct llrblink* link  = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);
	int count = 0;

	while (link != NULL) {
		struct ollrb_node* node = container_of(link, struct ollrb_node, link);

		/* first remove the link from the tree, no memory change */
		ollrb->root = llrb_remove_v(ollrb->root, link, ollrb_llrblink_compare, ollrb->content_traits.__compare_to);
		ollrb_reassociate(ollrb);

		/* second destroy the reference */
		ollrb->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, ollrb->allocator);

		/* third dealloc the node */
		allocator_dealloc(ollrb->allocator, node);

		ollrb->size --;
		count ++;

		link  = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);
	}

	return count;
}


void ollrb_itr_remove(_object* o, iterator itr) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct ollrb_node* node = container_of(oitr->current, struct ollrb_node, link);

	dbg_assert(oitr->__cast == ollrb_itr_cast);
	dbg_assert(oitr->current != NULL);

	if (oitr->current == &ollrb->sentinel)
		return;

	ollrb->root = llrb_remove_v(ollrb->root, &node->link, ollrb_llrblink_compare, ollrb->content_traits.__compare_to);
	ollrb_reassociate(ollrb);

	ollrb->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, ollrb->allocator);
	allocator_dealloc(ollrb->allocator, node);

	/* invalidate the iterator */
	oitr->current = NULL;

	ollrb->size --;
}

