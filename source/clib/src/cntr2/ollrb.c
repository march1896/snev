#include <string.h>

#include <cntr2/iset.h>
#include <cntr2/iitr.h>
#include <cntr2/ifactory.h>
#include <cntr2/ollrb.h>
#include <cntr2/oallocator.h>

#include <util/llrb_link.h>

/* this module defines a left-lean red black tree container, which implements iset interface. */

enum llrb_interfaces {
	e_set,
	e_mset,
	e_l_count
};

struct ollrb_node {
	struct llrb_link              link;

	const void*                   reference;
};


struct ollrb_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	iobject                       __iftable[1];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     allocator;

	struct llrb_link*             current;
};

/* binary search tree */
struct ollrb {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	/* just a sentinel to represent the end of the tree, the maximum element of the tree */
	struct llrb_link              sentinel;
	/* __root == __sentinel.left */
	struct llrb_link*             root;

	int                           size;
	pf_ref_compare                ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;      

	struct ollrb_itr              itr_begin;
	struct ollrb_itr              itr_end;
};

static struct iset_vtable __iset_vtable = {
	ollrb_destroy,          /* __destroy */
	ollrb_clear,            /* __clear */
	ollrb_clear_v,          /* __clear */
	ollrb_size,             /* __size */
	ollrb_empty,            /* __empty */
	ollrb_insert_s,         /* __insert */
	ollrb_contains,         /* __contains */
	ollrb_remove,           /* __remove */

	ollrb_itr_create,       /* __itr_create */
	ollrb_itr_assign,       /* __itr_assign */
	ollrb_itr_find,         /* __itr_find */
	ollrb_itr_remove,       /* __itr_remove */
	ollrb_itr_begin,        /* __itr_begin */
	ollrb_itr_end           /* __itr_end */
};

static struct imset_vtable __imset_vtable = {
	ollrb_destroy,          /* __destroy */
	ollrb_clear,            /* __clear */
	ollrb_clear_v,          /* __clear */
	ollrb_size,             /* __size */
	ollrb_empty,            /* __empty */
	ollrb_insert_m,         /* __insert */
	ollrb_contains,         /* __contains */
	ollrb_count,            /* __count */
	ollrb_remove,           /* __remove */

	ollrb_itr_create,       /* __itr_create */
	ollrb_itr_assign,       /* __itr_assign */
	ollrb_itr_find_lower,   /* __itr_find_lower */
	ollrb_itr_find_upper,   /* __itr_find_upper */
	ollrb_itr_remove,       /* __itr_remove */
	ollrb_itr_begin,        /* __itr_begin */
	ollrb_itr_end           /* __itr_end */
};

static unknown ollrb_itr_cast(unknown x, unique_id inf_id);
static unknown ollrb_cast(unknown x, unique_id intf_id);

static void ollrb_itr_destroy(iterator citr);
static iterator ollrb_itr_clone(const_iterator citr);
static bool ollrb_itr_equals(const_iterator a, const_iterator b);
static const void* ollrb_itr_get_ref(const_iterator citr);
static void ollrb_itr_set_ref(iterator citr, const void* n_ref);
static void ollrb_itr_to_next(iterator citr);
static void ollrb_itr_to_prev(iterator citr);

static struct itr_bidirectional_vtable __itr_vtable = {
	ollrb_itr_destroy,      /* __destroy */
	ollrb_itr_clone,        /* __clone   */
	ollrb_itr_equals,       /* __equals  */
	ollrb_itr_get_ref,      /* __get_ref */
	ollrb_itr_set_ref,      /* __set_ref */
	ollrb_itr_to_next,      /* __to_next */
	ollrb_itr_to_prev       /* __to_prev */
};


static void ollrb_itr_destroy(object* citr) {
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
	/* TODO: this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool ollrb_itr_equals(const_iterator a, const_iterator b) {
	const struct ollrb_itr* itr_a = (const struct ollrb_itr*)a;
	const struct ollrb_itr* itr_b = (const struct ollrb_itr*)b;

	dbg_assert(__is_object((unknown)a));
	dbg_assert(__is_object((unknown)b));
	dbg_assert(itr_a->__cast == ollrb_itr_cast);
	dbg_assert(itr_b->__cast == ollrb_itr_cast);

	return itr_a->current == itr_b->current;
}

static const void* ollrb_itr_get_ref(const_iterator citr) {
	const struct ollrb_itr* itr   = (const struct ollrb_itr*)citr;
	const struct ollrb_node* node = NULL;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct ollrb_node, link);
	return node->reference;
}

static void ollrb_itr_set_ref(iterator citr, const void* n_ref) {
	/* llrb does not permit to set ref, which would destroy the inner data structure. */
	/*
	struct ollrb_itr* itr   = (struct ollrb_itr*)citr;
	struct ollrb_node* node = NULL;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = container_of(itr->__current, struct ollrb_node, link);
	node->reference = n_ref;
	*/

	unused(citr);
	unused(n_ref);

	dbg_assert(false);

	return;
}

static void ollrb_itr_to_next(object* citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = llrb_successor(itr->current, false);
}

static void ollrb_itr_to_prev(object* citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = llrb_predesessor(itr->current, false);
}

static unknown ollrb_cast(unknown x, unique_id intf_id) {
	struct ollrb* o = (struct ollrb*)x;

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

static unknown ollrb_itr_cast(unknown x, unique_id inf_id) {
	struct ollrb_itr* itr = (struct ollrb_itr*)x;

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

static int ollrb_compare_v(const struct llrb_link* a, const struct llrb_link* b, void* param) {
	pf_ref_compare ref_comp = (pf_ref_compare)param;

	struct ollrb_node* node_a = container_of(a, struct ollrb_node, link);
	struct ollrb_node* node_b = container_of(b, struct ollrb_node, link);

	return ref_comp(node_a->reference, node_b->reference);
}

object* ollrb_create(pf_ref_compare ref_compare) {
	return ollrb_create_v(ref_compare, __global_default_allocator);
}

static void ollrb_itr_com_init(struct ollrb_itr* itr, struct ollrb* list);
object* ollrb_create_v(pf_ref_compare ref_compare, allocator alc) {
	struct ollrb* ollrb = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	ollrb = (struct ollrb*)allocator_alloc(alc, sizeof(struct ollrb));

	ollrb->__offset = ollrb;
	ollrb->__cast   = ollrb_cast;
	
	ollrb->__iftable[e_set].__offset = (address)e_set;
	ollrb->__iftable[e_set].__vtable = &__iset_vtable;
	ollrb->__iftable[e_mset].__offset = (address)e_mset;
	ollrb->__iftable[e_mset].__vtable = &__imset_vtable;

	ollrb->size      = 0;
	ollrb->ref_comp  = ref_compare;
	ollrb->root      = NULL;
	ollrb->sentinel.left   = NULL;
	ollrb->sentinel.right  = NULL;
	ollrb->sentinel.parent = NULL;
	ollrb->sentinel.color  = 55; /* TODO: how to handle this color? */


	ollrb->allocator = alc;
	ollrb->allocator_join_ondispose = managed_allocator;

	/* initialize begin/end iterators, the position is reassigned when each query */
	ollrb_itr_com_init(&ollrb->itr_begin, ollrb);
	ollrb_itr_com_init(&ollrb->itr_end, ollrb);

	return (object*)ollrb;
}

/* from ifactory.h  */
object* cntr_create_ollrb(pf_ref_compare comp) {
	return ollrb_create(comp);
}
object* cntr_create_ollrb_v(pf_ref_compare comp, allocator alc) {
	return ollrb_create_v(comp, alc);
}

void ollrb_destroy(object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;
	allocator alc = ollrb->allocator;
	bool join_alc = ollrb->allocator_join_ondispose;

	ollrb_clear(o);

	allocator_dealloc(alc, ollrb);

	if (join_alc) {
		allocator_join(alc);
	}
}

typedef void (*pf_per_link_operation)(struct llrb_link* link, void* param);
static void llrb_traverse(struct llrb_link* cur, pf_per_link_operation cb, void* param) {
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

static void ollrblink_dispose(struct llrb_link* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
	struct ollrb* ollrb     = (struct ollrb*)param;

	/* delete the node it self */
	allocator_dealloc(ollrb->allocator, node);
}

void ollrb_clear(object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	llrb_traverse(ollrb->root, ollrblink_dispose, (void*)ollrb);

	ollrb->root = NULL;
	ollrb_reassociate(ollrb);
	ollrb->size = 0;
}

struct clear_v_params {
	pf_ref_dispose_v callback;
	void*            context;
	struct ollrb*    container;
};
static void ollrblink_dispose_v(struct llrb_link* link, void* param) {
	struct ollrb_node* node     = container_of(link, struct ollrb_node, link);
	struct clear_v_params* dp = (struct clear_v_params*)param;

	/* first dispose the reference */
	if (dp->callback) {
		dp->callback((void*)node->reference, dp->context);
	}

	/* delete the node it self */
	dbg_assert(dp && dp->container && dp->container->allocator);
	allocator_dealloc(dp->container->allocator, node);
}

void ollrb_clear_v(object* o, pf_ref_dispose_v dispose, void* context) {
	struct ollrb* ollrb = (struct ollrb*)o;
	struct clear_v_params clear_params = {
		dispose, context, ollrb
	};

	llrb_traverse(ollrb->root, ollrblink_dispose_v, (void*)&clear_params);

	ollrb->root = NULL;
	ollrb_reassociate(ollrb);
	ollrb->size = 0;
}

int ollrb_size(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	return ollrb->size;
}

bool ollrb_empty(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;
	return ollrb->size == 0;
}

/* initialize all part of an iterator except the __current position */
static void ollrb_itr_com_init(struct ollrb_itr* itr, struct ollrb* list) {
	itr->__offset = itr;
	itr->__cast   = ollrb_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->allocator = list->allocator;
	/* itr->__current = NULL; */
}

const_iterator ollrb_itr_begin(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb->itr_begin.current = llrb_min(ollrb->root);

	return (iterator)&ollrb->itr_begin;
}

const_iterator ollrb_itr_end(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb->itr_end.current = &ollrb->sentinel;

	return (iterator)&ollrb->itr_end;
}

iterator ollrb_itr_create(const object* o, itr_pos pos) {
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
	else {
		/* sentinel will be the maximum element in the tree */
		n_itr->current = &ollrb->sentinel;
	}
	
	return (object*)n_itr;
}

void ollrb_itr_assign(const object* o, iterator itr, itr_pos pos) {
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
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->current = &ollrb->sentinel;
	}
}

struct direct_s {
	pf_ref_compare  comp;
	const void* target;
	const struct llrb_link* candidate; /* only useful for multiple instances */
};

static int ollrb_direct(const struct llrb_link* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = dir->comp(node->reference, dir->target);

	if (compr == 0)
		return 0;
	else if (compr < 0) /* we should expose right side */
		return 1;
	else 
		return -1;
}

static int ollrb_direct_lower(const struct llrb_link* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = dir->comp(node->reference, dir->target);

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

static int ollrb_direct_upper(const struct llrb_link* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = dir->comp(node->reference, dir->target);

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

void ollrb_itr_find(const object* o, iterator itr, const void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->ref_comp, __ref, NULL };
	struct llrb_link* link  = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);

	dbg_assert(dir.candidate == NULL);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) {
		oitr->current = link;
	}
	else {
		oitr->current = &ollrb->sentinel;
	}
}

void ollrb_itr_find_lower(const object* o, iterator itr, const void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->ref_comp, __ref, NULL };
	struct llrb_link* link  = llrb_search(ollrb->sentinel.left, ollrb_direct_lower, &dir);

	dbg_assert(link == NULL); /* we will always direct down */
	/* TODO: remove const cast */
	link = (struct llrb_link*)dir.candidate;    /* the last candidate, the most closed to leaf one, is what we want */

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) {
		oitr->current = link;
	}
	else {
		oitr->current = &ollrb->sentinel;
	}
}

void ollrb_itr_find_upper(const object* o, iterator itr, const void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->ref_comp, __ref, NULL };
	struct llrb_link* link  = llrb_search(ollrb->sentinel.left, ollrb_direct_upper, &dir);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct llrb_link*)dir.candidate;

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) { 
		oitr->current = link;
	}
	else {
		oitr->current = &ollrb->sentinel;
	}
}

void* ollrb_insert_s(object* o, const void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->allocator, sizeof(struct ollrb_node));
	struct llrb_link* duplicated = NULL;

	node->reference = __ref;
	ollrb->root = llrb_insert_sv(ollrb->root, &node->link, ollrb_compare_v, ollrb->ref_comp, &duplicated);
	ollrb_reassociate(ollrb);

	if (duplicated != NULL) {
		struct ollrb_node* dup_node = container_of(duplicated, struct ollrb_node, link);
		const void* old_ref = dup_node->reference;
		dbg_assert(ollrb_compare_v(&node->link, duplicated, ollrb->ref_comp) == 0);

		allocator_dealloc(ollrb->allocator, node);

		/* update the reference */
		dup_node->reference = __ref;
		return (void*)old_ref;
	}
	
	ollrb->size ++;

	return NULL;
}

void ollrb_insert_m(object* o, const void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->allocator, sizeof(struct ollrb_node));

	node->reference = __ref;
	ollrb->root = llrb_insert_v(ollrb->root, &node->link, ollrb_compare_v, ollrb->ref_comp);
	ollrb_reassociate(ollrb);

	ollrb->size ++;
	return;
}

bool ollrb_contains(const object* o, const void* __ref) {
	struct ollrb* ollrb    = (struct ollrb*)o;
	struct direct_s   dir  = { ollrb->ref_comp, __ref, NULL };
	struct llrb_link* link = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);

	if (link != NULL) {
		return true;
	}
	
	return false;
}

int ollrb_count(const object* o, const void* __ref) {
	struct ollrb*     ollrb    = (struct ollrb*)o;
	struct direct_s   dir      = { ollrb->ref_comp, __ref, NULL };
	const struct llrb_link* lb = llrb_search(ollrb->sentinel.left, ollrb_direct_lower, &dir);
	dbg_assert(lb == NULL);
	lb = (dir.candidate);

	dir.candidate = NULL;
	if (lb != NULL) {
		const struct llrb_link* ub = llrb_search(ollrb->sentinel.left, ollrb_direct_upper, &dir);
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

bool ollrb_remove(object* o, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct direct_s   dir   = { ollrb->ref_comp, __ref, NULL };
	struct llrb_link* link   = llrb_search(ollrb->sentinel.left, ollrb_direct, &dir);

	if (link != NULL) {
		struct ollrb_node* node = container_of(link, struct ollrb_node, link);

		ollrb->root = llrb_remove_v(ollrb->root, link, ollrb_compare_v, ollrb->ref_comp);
		ollrb_reassociate(ollrb);

		allocator_dealloc(ollrb->allocator, node);

		ollrb->size --;

		return true;
	}

	return false;
}

void* ollrb_itr_remove(object* o, iterator itr) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct ollrb_node* node = container_of(oitr->current, struct ollrb_node, link);
	const void* obj_ref      = node->reference;

	dbg_assert(oitr->__cast == ollrb_itr_cast);
	dbg_assert(oitr->current != NULL);

	ollrb->root = llrb_remove_v(ollrb->root, &node->link, ollrb_compare_v, ollrb->ref_comp);
	ollrb_reassociate(ollrb);

	/* we only free the node pointer, not the reference, the reference is returned to the client */
	allocator_dealloc(ollrb->allocator, node);

	ollrb->size --;

	return (void*)obj_ref;
}

