#include <string.h>

#include "cntr2/iset.h"
#include "cntr2/imset.h"
#include "cntr2/iitr.h"

#include "cntr2/ifactory.h"
#include "cntr2/oallocator.h"

#include "util/llrb_link.h"

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
	allocator                     __allocator;

	struct llrb_link*             __current;
};

/* binary search tree */
struct ollrb {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	/* just a sentinel to represent the end of the tree, the maximum element of the tree */
	struct llrb_link              __sentinel;
	/* __root == __sentinel.left */
	struct llrb_link*             __root;

	int                           __size;
	pf_compare                    __ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     __allocator;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_dispose                    __dispose;         

	struct ollrb_itr              __itr_begin;
	struct ollrb_itr              __itr_end;
};

static object*  ollrb_create          (pf_compare ref_comp);
static object*  ollrb_create_v        (pf_compare ref_comp, allocator alc, pf_dispose dispose);
static void     ollrb_destroy         (object* o);
static void     ollrb_clear           (object* o);
static int      ollrb_size            (const object* o);
static bool     ollrb_empty           (const object* o);
static bool     ollrb_insert_s        (object* o, void* __ref);
static void     ollrb_insert_m        (object* o, void* __ref);
static bool     ollrb_contains        (const object* o, void* __ref);
static int      ollrb_count           (const object* o, void* __ref);
static bool     ollrb_remove          (object* o, void* __ref);

static iterator ollrb_itr_create      (const object* o, itr_pos pos);
static void     ollrb_itr_assign      (const object* o, iterator itr, itr_pos pos);
static void     ollrb_itr_find        (const object* o, iterator itr, void* __ref);
static void     ollrb_itr_find_lower  (const object* o, iterator itr, void* __ref);
static void     ollrb_itr_find_upper  (const object* o, iterator itr, void* __ref);

static void*    ollrb_itr_remove      (object* o, iterator itr);

static const_iterator ollrb_itr_begin (const object* o);
static const_iterator ollrb_itr_end   (const object* o);

/* factory method, the only public function in this file */
object* create_llrb_container(pf_compare ref_compare) {
	return ollrb_create(ref_compare);
}
object* create_llrb_container_v(pf_compare ref_compare, allocator alc, pf_dispose dispose) {
	return ollrb_create_v(ref_compare, alc, dispose);
}

static struct iset_vtable __iset_vtable = {
	ollrb_destroy,          /* __destroy */
	ollrb_clear,            /* __clear */
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
	allocator_dealloc(itr->__allocator, itr);
}

static iterator ollrb_itr_clone(const_iterator citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;
	struct ollrb_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == ollrb_itr_cast);

	/* destroy itself */
	n_itr = (struct ollrb_itr*)allocator_alloc(itr->__allocator, sizeof(struct ollrb_itr));

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

	return itr_a->__current == itr_b->__current;
}

static const void* ollrb_itr_get_ref(const_iterator citr) {
	const struct ollrb_itr* itr   = (const struct ollrb_itr*)citr;
	const struct ollrb_node* node = NULL;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = container_of(itr->__current, struct ollrb_node, link);
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
	dbg_assert(itr->__current != NULL);

	itr->__current = llrb_successor(itr->__current, false);
}

static void ollrb_itr_to_prev(object* citr) {
	struct ollrb_itr* itr = (struct ollrb_itr*)citr;

	dbg_assert(itr->__cast == ollrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = llrb_predesessor(itr->__current, false);
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
	pf_compare ref_comp = (pf_compare)param;

	struct ollrb_node* node_a = container_of(a, struct ollrb_node, link);
	struct ollrb_node* node_b = container_of(b, struct ollrb_node, link);

	return ref_comp(node_a->reference, node_b->reference);
}

static object* ollrb_create(pf_compare ref_compare) {
	return ollrb_create_v(ref_compare, global_sysd_allocator, NULL);
}

static void ollrb_itr_com_init(struct ollrb_itr* itr, struct ollrb* list);
static object* ollrb_create_v(pf_compare ref_compare, allocator alc, pf_dispose dispose) {
	struct ollrb* ollrb = NULL;

	dbg_assert(alc != NULL);

	ollrb = (struct ollrb*)allocator_alloc(alc, sizeof(struct ollrb));

	ollrb->__offset = ollrb;
	ollrb->__cast   = ollrb_cast;
	
	ollrb->__iftable[e_set].__offset = (address)e_set;
	ollrb->__iftable[e_set].__vtable = &__iset_vtable;
	ollrb->__iftable[e_mset].__offset = (address)e_mset;
	ollrb->__iftable[e_mset].__vtable = &__imset_vtable;

	ollrb->__size      = 0;
	ollrb->__ref_comp  = ref_compare;
	ollrb->__root      = NULL;
	ollrb->__sentinel.left   = NULL;
	ollrb->__sentinel.right  = NULL;
	ollrb->__sentinel.parent = NULL;
	ollrb->__sentinel.color  = 55; /* TODO: how to handle this color? */

	ollrb->__allocator = alc;

	ollrb->__dispose   = dispose;

	/* initialize begin/end iterators, the position is reassigned when each query */
	ollrb_itr_com_init(&ollrb->__itr_begin, ollrb);
	ollrb_itr_com_init(&ollrb->__itr_end, ollrb);

	return (object*)ollrb;
}

/* from ifactory.h  */
object* cntr_create_ollrb(pf_compare comp) {
	return ollrb_create(comp);
}
object* cntr_create_ollrb_v(pf_compare comp, allocator alc, pf_dispose dispose) {
	return ollrb_create_v(comp, alc, dispose);
}


static void per_link_dispose(struct llrb_link* link, void* param) {
	struct ollrb_node* node = container_of(link, struct ollrb_node, link);
	struct ollrb* ollrb     = (struct ollrb*)param;

	/* first dispose the reference */
	if (ollrb->__dispose) {
		ollrb->__dispose((void*)node->reference);
	}
	
	/* delete the node it self */
	allocator_dealloc(ollrb->__allocator, node);
}

static void ollrb_destroy(object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb_clear(o);
	allocator_dealloc(ollrb->__allocator, ollrb);
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
	ollrb->__sentinel.left = ollrb->__root;

	if (ollrb->__root != NULL)
		ollrb->__root->parent = &ollrb->__sentinel;
}

static void ollrb_clear(object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	llrb_traverse(ollrb->__root, per_link_dispose, (void*)ollrb);

	ollrb->__root = NULL;
	ollrb_reassociate(ollrb);
	ollrb->__size = 0;
}

static int ollrb_size(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	return ollrb->__size;
}

static bool ollrb_empty(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;
	return ollrb->__size == 0;
}

/* initialize all part of an iterator except the __current position */
static void ollrb_itr_com_init(struct ollrb_itr* itr, struct ollrb* list) {
	itr->__offset = itr;
	itr->__cast   = ollrb_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__allocator = list->__allocator;
	/* itr->__current = NULL; */
}

static const_iterator ollrb_itr_begin(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb->__itr_begin.__current = llrb_min(ollrb->__root);

	return (iterator)&ollrb->__itr_begin;
}

static const_iterator ollrb_itr_end(const object* o) {
	struct ollrb* ollrb = (struct ollrb*)o;

	ollrb->__itr_end.__current = &ollrb->__sentinel;

	return (iterator)&ollrb->__itr_end;
}

static iterator ollrb_itr_create(const object* o, itr_pos pos) {
	struct ollrb* ollrb = (struct ollrb*)o;
	struct ollrb_itr* n_itr = (struct ollrb_itr*)
		allocator_alloc(ollrb->__allocator, sizeof(struct ollrb_itr));

	ollrb_itr_com_init(n_itr, ollrb);

	if (pos == itr_begin) {
		n_itr->__current = llrb_min(ollrb->__root);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = &ollrb->__sentinel;
	}
	
	return (object*)n_itr;
}

static void ollrb_itr_assign(const object* o, iterator itr, itr_pos pos) {
	struct ollrb* ollrb = (struct ollrb*)o;
	struct ollrb_itr* n_itr = (struct ollrb_itr*)itr;

	ollrb_itr_com_init(n_itr, ollrb);

	if (pos == itr_begin) {
		n_itr->__current = llrb_min(ollrb->__root);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = &ollrb->__sentinel;
	}
}

struct direct_s {
	pf_compare  comp;
	const void* target;
	const struct llrb_link* candidate; /* only useful for multiple instances */
};

/* TODO: totally wrong, using directly compare instead of the comparison callback */
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

static void ollrb_itr_find(const object* o, iterator itr, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->__ref_comp, __ref, NULL };
	struct llrb_link* link  = llrb_search(ollrb->__sentinel.left, ollrb_direct, &dir);

	dbg_assert(dir.candidate == NULL);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) {
		oitr->__current = link;
	}
	else {
		oitr->__current = &ollrb->__sentinel;
	}
}

static void ollrb_itr_find_lower(const object* o, iterator itr, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->__ref_comp, __ref, NULL };
	struct llrb_link* link  = llrb_search(ollrb->__sentinel.left, ollrb_direct_lower, &dir);

	dbg_assert(link == NULL); /* we will always direct down */
	/* TODO: remove const cast */
	link = (struct llrb_link*)dir.candidate;    /* the last candidate, the most closed to leaf one, is what we want */

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) {
		oitr->__current = link;
	}
	else {
		oitr->__current = &ollrb->__sentinel;
	}
}

static void ollrb_itr_find_upper(const object* o, iterator itr, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct direct_s   dir   = { ollrb->__ref_comp, __ref, NULL };
	struct llrb_link* link  = llrb_search(ollrb->__sentinel.left, ollrb_direct_upper, &dir);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct llrb_link*)dir.candidate;

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) { 
		oitr->__current = link;
	}
	else {
		oitr->__current = &ollrb->__sentinel;
	}
}

static bool ollrb_insert_s(object* o, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->__allocator, sizeof(struct ollrb_node));
	bool duplicated;

	node->reference = __ref;
	ollrb->__root = llrb_insert_sv(ollrb->__root, &node->link, ollrb_compare_v, ollrb->__ref_comp, &duplicated);
	ollrb_reassociate(ollrb);

	if (duplicated == true) {
		allocator_dealloc(ollrb->__allocator, node);
		return false;
	}
	
	ollrb->__size ++;

	return true;
}

static void ollrb_insert_m(object* o, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_node* node = (struct ollrb_node*)
		allocator_alloc(ollrb->__allocator, sizeof(struct ollrb_node));

	node->reference = __ref;
	ollrb->__root = llrb_insert_v(ollrb->__root, &node->link, ollrb_compare_v, ollrb->__ref_comp);
	ollrb_reassociate(ollrb);

	ollrb->__size ++;
	return;
}

static bool ollrb_contains(const object* o, void* __ref) {
	struct ollrb* ollrb    = (struct ollrb*)o;
	struct direct_s   dir  = { ollrb->__ref_comp, __ref, NULL };
	struct llrb_link* link = llrb_search(ollrb->__sentinel.left, ollrb_direct, &dir);

	if (link != NULL) {
		return true;
	}
	
	return false;
}

static int ollrb_count(const object* o, void* __ref) {
	struct ollrb*     ollrb    = (struct ollrb*)o;
	struct direct_s   dir      = { ollrb->__ref_comp, __ref, NULL };
	const struct llrb_link* lb = llrb_search(ollrb->__sentinel.left, ollrb_direct_lower, &dir);
	dbg_assert(lb == NULL);
	lb = (dir.candidate);

	dir.candidate = NULL;
	if (lb != NULL) {
		const struct llrb_link* ub = llrb_search(ollrb->__sentinel.left, ollrb_direct_upper, &dir);
		int count = 0;

		dbg_assert(ub == NULL);
		ub = (dir.candidate);
		
		if (ub == NULL) 
			ub = &ollrb->__sentinel;

		while (lb != ub) {
			count ++;
			lb = llrb_successor(lb, false);
		}

		return count;
	}

	return 0;
}

static bool ollrb_remove(object* o, void* __ref) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct direct_s   dir   = { ollrb->__ref_comp, __ref, NULL };
	struct llrb_link* link   = llrb_search(ollrb->__sentinel.left, ollrb_direct, &dir);

	if (link != NULL) {
		struct ollrb_node* node = container_of(link, struct ollrb_node, link);

		ollrb->__root = llrb_remove_v(ollrb->__root, link, ollrb_compare_v, ollrb->__ref_comp);
		ollrb_reassociate(ollrb);

		allocator_dealloc(ollrb->__allocator, node);

		ollrb->__size --;

		return true;
	}

	return false;
}

static void* ollrb_itr_remove(object* o, iterator itr) {
	struct ollrb* ollrb     = (struct ollrb*)o;
	struct ollrb_itr* oitr  = (struct ollrb_itr*)itr;
	struct ollrb_node* node = container_of(oitr->__current, struct ollrb_node, link);
	const void* obj_ref      = node->reference;

	dbg_assert(oitr->__cast == ollrb_itr_cast);
	dbg_assert(oitr->__current != NULL);

	ollrb->__root = llrb_remove_v(ollrb->__root, &node->link, ollrb_compare_v, ollrb->__ref_comp);
	ollrb_reassociate(ollrb);

	/* we only free the node pointer, not the reference, the reference is returned to the client */
	allocator_dealloc(ollrb->__allocator, node);

	ollrb->__size --;

	return (void*)obj_ref;
}

