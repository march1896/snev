#include <string.h>

#include <cntr2/iset.h>
#include <cntr2/iitr.h>
#include <cntr2/ifactory.h>
#include <cntr2/osplay.h>
#include <cntr2/oallocator.h>

#include <util/splay_link.h>

/* this module defines a left-lean red black tree container, which implements iset interface. */

enum splay_interfaces {
	e_set,
	e_mset,
	e_l_count
};

struct osplay_node {
	struct splay_link             link;

	const void*                   reference;
};

struct osplay_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	iobject                       __iftable[1];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     __allocator;

	struct splay_link*            __current;
};

/* binary search tree */
struct osplay {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	/* just a sentinel to represent the end of the tree, the maximum element of the tree */
	struct splay_link             __sentinel;
	/* __root == __sentinel.left */
	struct splay_link*            __root;

	int                           __size;
	pf_compare                    __ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     __allocator;
	bool                          __allocator_join_ondispose;      

	struct osplay_itr             __itr_begin;
	struct osplay_itr             __itr_end;
};

static struct iset_vtable __iset_vtable = {
	splayset_destroy,          /* __destroy */
	splayset_clear,            /* __clear */
	splayset_clear_v,          /* __clear_v */
	splayset_size,             /* __size */
	splayset_empty,            /* __empty */
	splayset_insert_s,         /* __insert */
	splayset_contains,         /* __contains */
	splayset_remove,           /* __remove */

	splayset_itr_create,       /* __itr_create */
	splayset_itr_assign,       /* __itr_assign */
	splayset_itr_find_s,         /* __itr_find */
	splayset_itr_remove,       /* __itr_remove */
	splayset_itr_begin,        /* __itr_begin */
	splayset_itr_end           /* __itr_end */
};

static struct imset_vtable __imset_vtable = {
	splayset_destroy,          /* __destroy */
	splayset_clear,            /* __clear */
	splayset_clear_v,          /* __clear_v */
	splayset_size,             /* __size */
	splayset_empty,            /* __empty */
	splayset_insert_m,         /* __insert */
	splayset_contains,         /* __contains */
	splayset_count_m,            /* __count */
	splayset_remove,           /* __remove */

	splayset_itr_create,       /* __itr_create */
	splayset_itr_assign,       /* __itr_assign */
	splayset_itr_find_lower_m,   /* __itr_find_lower */
	splayset_itr_find_upper_m,   /* __itr_find_upper */
	splayset_itr_remove,       /* __itr_remove */
	splayset_itr_begin,        /* __itr_begin */
	splayset_itr_end           /* __itr_end */
};

static unknown osplay_itr_cast(unknown x, unique_id inf_id);
static unknown osplay_cast(unknown x, unique_id intf_id);

static void osplay_itr_destroy(iterator citr);
static iterator osplay_itr_clone(const_iterator citr);
static bool osplay_itr_equals(const_iterator a, const_iterator b);
static const void* osplay_itr_get_ref(const_iterator citr);
static void osplay_itr_set_ref(iterator citr, const void* n_ref);
static void osplay_itr_to_next(iterator citr);
static void osplay_itr_to_prev(iterator citr);

static struct itr_bidirectional_vtable __itr_vtable = {
	osplay_itr_destroy,      /* __destroy */
	osplay_itr_clone,        /* __clone   */
	osplay_itr_equals,       /* __equals  */
	osplay_itr_get_ref,      /* __get_ref */
	osplay_itr_set_ref,      /* __set_ref */
	osplay_itr_to_next,      /* __to_next */
	osplay_itr_to_prev       /* __to_prev */
};


static void osplay_itr_destroy(object* citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == osplay_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->__allocator, itr);
}

static iterator osplay_itr_clone(const_iterator citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;
	struct osplay_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == osplay_itr_cast);

	/* destroy itself */
	n_itr = (struct osplay_itr*)allocator_alloc(itr->__allocator, sizeof(struct osplay_itr));

	memcpy (n_itr, itr, sizeof(struct osplay_itr));
	/* TODO: this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool osplay_itr_equals(const_iterator a, const_iterator b) {
	const struct osplay_itr* itr_a = (const struct osplay_itr*)a;
	const struct osplay_itr* itr_b = (const struct osplay_itr*)b;

	dbg_assert(__is_object((unknown)a));
	dbg_assert(__is_object((unknown)b));
	dbg_assert(itr_a->__cast == osplay_itr_cast);
	dbg_assert(itr_b->__cast == osplay_itr_cast);

	return itr_a->__current == itr_b->__current;
}

static const void* osplay_itr_get_ref(const_iterator citr) {
	const struct osplay_itr* itr   = (const struct osplay_itr*)citr;
	const struct osplay_node* node = NULL;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = container_of(itr->__current, struct osplay_node, link);
	return node->reference;
}

static void osplay_itr_set_ref(iterator citr, const void* n_ref) {
	/* splay does not permit to set ref, which would destroy the inner data structure. */
	/*
	struct osplay_itr* itr   = (struct osplay_itr*)citr;
	struct osplay_node* node = NULL;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = container_of(itr->__current, struct osplay_node, link);
	node->reference = n_ref;
	*/

	unused(citr);
	unused(n_ref);

	dbg_assert(false);

	return;
}

static void osplay_itr_to_next(object* citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = splay_successor(itr->__current, false);
}

static void osplay_itr_to_prev(object* citr) {
	struct osplay_itr* itr = (struct osplay_itr*)citr;

	dbg_assert(itr->__cast == osplay_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = splay_predesessor(itr->__current, false);
}

static unknown osplay_cast(unknown x, unique_id intf_id) {
	struct osplay* o = (struct osplay*)x;

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

static unknown osplay_itr_cast(unknown x, unique_id inf_id) {
	struct osplay_itr* itr = (struct osplay_itr*)x;

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

static int osplay_compare_v(const struct splay_link* a, const struct splay_link* b, void* param) {
	pf_compare ref_comp = (pf_compare)param;

	struct osplay_node* node_a = container_of(a, struct osplay_node, link);
	struct osplay_node* node_b = container_of(b, struct osplay_node, link);

	return ref_comp(node_a->reference, node_b->reference);
}

object* splayset_create(pf_compare ref_compare) {
	return splayset_create_v(ref_compare, __global_default_allocator);
}

static void osplay_itr_com_init(struct osplay_itr* itr, struct osplay* list);
object* splayset_create_v(pf_compare ref_compare, allocator alc) {
	struct osplay* osplay = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	osplay = (struct osplay*)allocator_alloc(alc, sizeof(struct osplay));

	osplay->__offset = osplay;
	osplay->__cast   = osplay_cast;
	
	osplay->__iftable[e_set].__offset = (address)e_set;
	osplay->__iftable[e_set].__vtable = &__iset_vtable;
	osplay->__iftable[e_mset].__offset = (address)e_mset;
	osplay->__iftable[e_mset].__vtable = &__imset_vtable;

	osplay->__size      = 0;
	osplay->__ref_comp  = ref_compare;
	osplay->__root      = NULL;
	osplay->__sentinel.left   = NULL;
	osplay->__sentinel.right  = NULL;
	osplay->__sentinel.parent = NULL;

	osplay->__allocator = alc;
	osplay->__allocator_join_ondispose = managed_allocator;

	/* initialize begin/end iterators, the position is reassigned when each query */
	osplay_itr_com_init(&osplay->__itr_begin, osplay);
	osplay_itr_com_init(&osplay->__itr_end, osplay);

	return (object*)osplay;
}

/* from ifactory.h  */
object* cntr_create_osplay(pf_compare comp) {
	return splayset_create(comp);
}
object* cntr_create_osplay_v(pf_compare comp, allocator alc) {
	return splayset_create_v(comp, alc);
}

void splayset_destroy(object* o) {
	struct osplay* osplay = (struct osplay*)o;
	allocator alc = osplay->__allocator;
	bool join_alc = osplay->__allocator_join_ondispose;

	splayset_clear(o);

	allocator_dealloc(alc, osplay);

	if (join_alc) {
		allocator_join(alc);
	}
}

typedef void (*pf_per_link_operation)(struct splay_link* link, void* param);
static void splay_traverse(struct splay_link* cur, pf_per_link_operation cb, void* param) {
	if (cur == NULL) return;

	splay_traverse(cur->left, cb, param);
	splay_traverse(cur->right, cb, param);
	cb(cur, param);
}

static inline void osplay_detach_sentinel(struct osplay* osplay) {
	if (osplay->__root != NULL)
		osplay->__root->parent = NULL;

	osplay->__sentinel.left = NULL;
}

/* we have to re-associate the sentinel and the root node after we change the tree */
static inline void osplay_attach_sentinel(struct osplay* osplay) {
	osplay->__sentinel.left = osplay->__root;

	if (osplay->__root != NULL)
		osplay->__root->parent = &osplay->__sentinel;
}

static void splaylink_dispose(struct splay_link* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct osplay* osplay = (struct osplay*)param;

	/* delete the node it self */
	allocator_dealloc(osplay->__allocator, node);
}
void splayset_clear(object* o) {
	struct osplay* osplay = (struct osplay*)o;

	splay_traverse(osplay->__root, splaylink_dispose, (void*)osplay);

	osplay->__root = NULL;
	osplay_attach_sentinel(osplay);
	osplay->__size = 0;
}

struct splaynode_clear_v {
	pf_ref_dispose_v callback;
	void*            context;
	struct osplay*   container;
};
static void splaylink_dispose_v(struct splay_link* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct splaynode_clear_v* dp = (struct splaynode_clear_v*)param;

	/* first dispose the reference */
	if (dp->callback) {
		dp->callback((void*)node->reference, dp->context);
	}

	/* delete the node it self */
	/* delete the node it self */
	dbg_assert(dp && dp->container && dp->container->__allocator);
	allocator_dealloc(dp->container->__allocator, node);
}
void splayset_clear_v(object* o, pf_ref_dispose_v dispose, void* context) {
	struct osplay* osplay = (struct osplay*)o;
	struct splaynode_clear_v dispose_context = {
		dispose, context, osplay
	};

	splay_traverse(osplay->__root, splaylink_dispose_v, (void*)&dispose_context);

	osplay->__root = NULL;
	osplay_attach_sentinel(osplay);
	osplay->__size = 0;
}

int splayset_size(const object* o) {
	struct osplay* osplay = (struct osplay*)o;

	return osplay->__size;
}

bool splayset_empty(const object* o) {
	struct osplay* osplay = (struct osplay*)o;
	return osplay->__size == 0;
}

/* initialize all part of an iterator except the __current position */
static void osplay_itr_com_init(struct osplay_itr* itr, struct osplay* list) {
	itr->__offset = itr;
	itr->__cast   = osplay_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__allocator = list->__allocator;
	/* itr->__current = NULL; */
}

const_iterator splayset_itr_begin(const object* o) {
	struct osplay* osplay = (struct osplay*)o;

	osplay->__itr_begin.__current = splay_min(osplay->__root);

	return (iterator)&osplay->__itr_begin;
}

const_iterator splayset_itr_end(const object* o) {
	struct osplay* osplay = (struct osplay*)o;

	osplay->__itr_end.__current = &osplay->__sentinel;

	return (iterator)&osplay->__itr_end;
}

iterator splayset_itr_create(const object* o, itr_pos pos) {
	struct osplay* osplay = (struct osplay*)o;
	struct osplay_itr* n_itr = (struct osplay_itr*)
		allocator_alloc(osplay->__allocator, sizeof(struct osplay_itr));

	osplay_itr_com_init(n_itr, osplay);

	if (pos == itr_begin) {
		n_itr->__current = splay_min(osplay->__root);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = &osplay->__sentinel;
	}
	
	return (object*)n_itr;
}

void splayset_itr_assign(const object* o, iterator itr, itr_pos pos) {
	struct osplay* osplay = (struct osplay*)o;
	struct osplay_itr* n_itr = (struct osplay_itr*)itr;

	osplay_itr_com_init(n_itr, osplay);

	if (pos == itr_begin) {
		n_itr->__current = splay_min(osplay->__root);
	}
	else {
		/* __sentinel will be the maximum element in the tree */
		n_itr->__current = &osplay->__sentinel;
	}
}

struct direct_s {
	pf_compare  comp;
	const void* target;
	const struct splay_link* candidate; /* only useful for multiple instances */
};

static int osplay_direct(const struct splay_link* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = dir->comp(node->reference, dir->target);

	if (compr == 0)
		return 0;
	else if (compr < 0) /* we should expose right side */
		return 1;
	else 
		return -1;
}

static int osplay_direct_lower(const struct splay_link* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
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

static int osplay_direct_upper(const struct splay_link* link, void* param) {
	struct osplay_node* node = container_of(link, struct osplay_node, link);
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

void splayset_itr_find_s(const object* o, iterator itr, const void* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct direct_s   dir    = { osplay->__ref_comp, __ref, NULL };
	struct splay_link* find_res = NULL; 
	
	osplay_detach_sentinel(osplay);
	find_res = splay_dynamic_search(&osplay->__root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	dbg_assert(dir.candidate == NULL);

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (find_res != NULL) {
		oitr->__current = find_res;
	}
	else {
		oitr->__current = &osplay->__sentinel;
	}
}

void splayset_itr_find_lower_m(const object* o, iterator itr, const void* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct direct_s   dir    = { osplay->__ref_comp, __ref, NULL };
	struct splay_link* link  = NULL;

	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->__root, osplay_direct_lower, &dir);
	osplay_attach_sentinel(osplay);

	dbg_assert(link == NULL); /* we will always direct down */
	/* TODO: remove const cast */
	link = (struct splay_link*)dir.candidate;    /* the last candidate, the most closed to leaf one, is what we want */

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) {
		oitr->__current = link;
	}
	else {
		oitr->__current = &osplay->__sentinel;
	}
}

void splayset_itr_find_upper_m(const object* o, iterator itr, const void* __ref) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct direct_s   dir    = { osplay->__ref_comp, __ref, NULL };
	struct splay_link* link  = NULL;
	
	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->__root, osplay_direct_upper, &dir);
	osplay_attach_sentinel(osplay);

	dbg_assert(link == NULL); /* we will always direct down */
	link = (struct splay_link*)dir.candidate;

	/* make sure the iterator type is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	if (link != NULL) { 
		oitr->__current = link;
	}
	else {
		oitr->__current = &osplay->__sentinel;
	}
}

void* splayset_insert_s(object* o, const void* __ref) {
	struct osplay* osplay     = (struct osplay*)o;
	struct osplay_node* node = (struct osplay_node*)
		allocator_alloc(osplay->__allocator, sizeof(struct osplay_node));
	struct splay_link* duplicated = NULL;

	node->reference = __ref;
	osplay_detach_sentinel(osplay);
	osplay->__root = splay_insert_sv(osplay->__root, &node->link, osplay_compare_v, osplay->__ref_comp, &duplicated);
	osplay_attach_sentinel(osplay);

	if (duplicated != NULL) {
		struct osplay_node* dup_node = container_of(duplicated, struct osplay_node, link);
		const void* old_ref = dup_node->reference;
		dbg_assert(osplay_compare_v(&node->link, duplicated, osplay->__ref_comp) == 0);
		allocator_dealloc(osplay->__allocator, node);

		/* update the reference */
		dup_node->reference = __ref;
		return (void*)old_ref;
	}
	
	osplay->__size ++;

	return NULL;
}

void splayset_insert_m(object* o, const void* __ref) {
	struct osplay* osplay     = (struct osplay*)o;
	struct osplay_node* node = (struct osplay_node*)
		allocator_alloc(osplay->__allocator, sizeof(struct osplay_node));

	node->reference = __ref;
	osplay_detach_sentinel(osplay);
	osplay->__root = splay_insert_v(osplay->__root, &node->link, osplay_compare_v, osplay->__ref_comp);
	osplay_attach_sentinel(osplay);

	osplay->__size ++;
	return;
}

bool splayset_contains(const object* o, const void* __ref) {
	struct osplay* osplay   = (struct osplay*)o;
	struct direct_s   dir   = { osplay->__ref_comp, __ref, NULL };
	struct splay_link* link = NULL;
	
	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->__root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	if (link != NULL) {
		return true;
	}
	
	return false;
}

int splayset_count_m(const object* o, const void* __ref) {
	struct osplay*     osplay   = (struct osplay*)o;
	struct direct_s   dir       = { osplay->__ref_comp, __ref, NULL };
	const struct splay_link* lb = NULL;
	
	osplay_detach_sentinel(osplay);
	lb = splay_dynamic_search(&osplay->__root, osplay_direct_lower, &dir);
	dbg_assert(lb == NULL);
	osplay_attach_sentinel(osplay);

	lb = (dir.candidate);
	dir.candidate = NULL;
	if (lb != NULL) {
		const struct splay_link* ub = splay_search(osplay->__root, osplay_direct_upper, &dir);
		int count = 0;

		dbg_assert(ub == NULL);
		ub = (dir.candidate);
		
		if (ub == NULL) 
			ub = &osplay->__sentinel;

		while (lb != ub) {
			count ++;
			lb = splay_successor(lb, false);
		}

		return count;
	}

	return 0;
}

bool splayset_remove(object* o, void* __ref) {
	struct osplay* osplay   = (struct osplay*)o;
	struct direct_s   dir   = { osplay->__ref_comp, __ref, NULL };
	struct splay_link* link = NULL;
	
	osplay_detach_sentinel(osplay);
	link = splay_dynamic_search(&osplay->__root, osplay_direct, &dir);
	osplay_attach_sentinel(osplay);

	if (link != NULL) {
		struct osplay_node* node = container_of(link, struct osplay_node, link);

		osplay_detach_sentinel(osplay);
		osplay->__root = splay_remove_v(osplay->__root, link, osplay_compare_v, osplay->__ref_comp);
		osplay_attach_sentinel(osplay);

		allocator_dealloc(osplay->__allocator, node);

		osplay->__size --;

		return true;
	}

	return false;
}

void* osplay_smap_insert(object* o, const void* key, void* value) {

}

void* osplay_smap_findbykey  (object* o, const void* key) {

}

void     osplay_mmap_insert     (object* o, const void* key, void* value);
int      osplay_mmap_count      (const object* o, const void* __ref);
bool     osplay_xmap_contans    (object* o, const void* key);
void*    osplay_xmap_remove     (object* o, void* key);


void* splayset_itr_remove(object* o, iterator itr) {
	struct osplay* osplay    = (struct osplay*)o;
	struct osplay_itr* oitr  = (struct osplay_itr*)itr;
	struct osplay_node* node = container_of(oitr->__current, struct osplay_node, link);
	const void* obj_ref      = node->reference;

	dbg_assert(oitr->__cast == osplay_itr_cast);
	dbg_assert(oitr->__current != NULL);

	osplay_detach_sentinel(osplay);
	osplay->__root = splay_remove_v(osplay->__root, &node->link, osplay_compare_v, osplay->__ref_comp);
	osplay_attach_sentinel(osplay);

	/* we only free the node pointer, not the reference, the reference is returned to the client */
	allocator_dealloc(osplay->__allocator, node);

	osplay->__size --;

	return (void*)obj_ref;
}

