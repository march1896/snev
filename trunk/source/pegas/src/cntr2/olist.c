#include <string.h>

#include "olist.h"
#include "iobject.h"
#include "ilist.h"
#include "iqueue.h"
#include "istack.h"
#include "iitr.h"
#include "ifactory.h"
#include "oallocator.h"
#include "util/list_link.h"
#include <memheap/heap_global.h>

/* this module defines double linked list(with sentinel) container, it implements 
 * ilist, iqueue, istack 
 * and we do not expose the object method currently, that is you have to use one of the interface 
 * above, but not the list directly. */

enum list_interfaces {
	e_object,
	e_list,
	e_queue,
	e_stack,
	e_l_count
};

struct o_dlist_node {
	struct listlink              link;

	unknown*                      reference;
};

struct o_dlist;
struct o_dlist_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	struct base_interface         __iftable[itr_interface_count];

	struct o_dlist*               container;
	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     allocator;

	struct listlink*             current;
};

struct o_dlist {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	struct listlink              sentinel;
	int                           size;

	unknown_traits                content_traits;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;

	struct o_dlist_itr            itr_begin;
	struct o_dlist_itr            itr_end;
};

static struct iobject_vtable __olist_iobject_vtable = {
	olist_destroy,          /* __destroy */
	olist_clone,            /* __clone */
	olist_equals,           /* __equals */
	olist_compare_to,       /* __compare_to */
	olist_hashcode,         /* __hashcode */
};

static struct ilist_vtable __olist_ilist_vtable = {
	olist_clear,            /* __clear */
	olist_foreach,          /* __foreach */
	olist_size,             /* __size */
	olist_empty,            /* __empty */
	olist_front,            /* __front */
	olist_back,             /* __back */
	olist_add_front,        /* __add_front */
	olist_add_back,         /* __add_back */
	olist_remove_front,     /* __remove_front */
	olist_remove_back,      /* __remove_back */
	olist_contains,         /* __contains */
	olist_remove,           /* __remove  */

	olist_itr_begin,        /* __itr_begin */
	olist_itr_end,          /* __itr_end */
	olist_itr_create,       /* __itr_create */
	olist_itr_assign,       /* __itr_assign */
	olist_itr_find,         /* __itr_find */
	olist_itr_remove,       /* __itr_remove */
	olist_insert_before,    /* __insert_before */
	olist_insert_after      /* __insert_after */
};

static struct iqueue_vtable __olist_iqueue_vtable = {
	olist_clear,            /* __clear */
	olist_foreach,          /* __foreach */
	olist_size,             /* __size */
	olist_empty,            /* __empty */
	olist_front,            /* __front */
	olist_back,             /* __back */
	olist_add_back,         /* __push */
	olist_remove_front,     /* __pop */
	olist_itr_create,       /* __itr_create */
	olist_itr_assign,       /* __itr_assign */
	olist_itr_begin,        /* __itr_begin */
	olist_itr_end           /* __itr_end */
};

static struct istack_vtable __olist_istack_vtable = {
	olist_clear,            /* __clear */
	olist_foreach,          /* __foreach */
	olist_size,             /* __size */
	olist_empty,            /* __empty */
	olist_back,             /* __top */
	olist_add_back,         /* __push */
	olist_remove_back,      /* __pop */
	olist_itr_create,       /* __itr_create */
	olist_itr_assign,       /* __itr_assign */
	olist_itr_begin,        /* __itr_begin */
	olist_itr_end           /* __itr_end */
};

static unknown* o_dlist_itr_cast(unknown* x, unique_id inf_id);
static unknown* o_dlist_cast(unknown* x, unique_id intf_id);

static void o_dlist_itr_destroy(iterator citr);
static iterator o_dlist_itr_clone(const_iterator citr);
static bool o_dlist_itr_equals(const_iterator a, const_iterator b);
static int o_dlist_itr_compare_to(const_iterator itr, const_iterator other);
static hashcode o_dlist_itr_hashcode(const_iterator itr);
static unknown* o_dlist_itr_get_obj(const_iterator citr);
static void o_dlist_itr_set_obj(iterator citr, const unknown* n_ref);
static const unknown* o_dlist_itr_get_ref(const_iterator citr);
static void o_dlist_itr_swap_ref(iterator citr, iterator other);
static void o_dlist_itr_to_next(iterator citr);
static void o_dlist_itr_to_prev(iterator citr);

static void o_dlist_itr_destroy(iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == o_dlist_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->allocator, itr);
}

static iterator o_dlist_itr_clone(const_iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;
	struct o_dlist_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == o_dlist_itr_cast);

	/* destroy itself */
	n_itr = (struct o_dlist_itr*)allocator_alloc(itr->allocator, sizeof(struct o_dlist_itr));

	memcpy(n_itr, itr, sizeof(struct o_dlist_itr));
	/* this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool o_dlist_itr_equals(const_iterator a, const_iterator b) {
	const struct o_dlist_itr* itr_a = (const struct o_dlist_itr*)a;
	const struct o_dlist_itr* itr_b = (const struct o_dlist_itr*)b;

	dbg_assert(__is_object(a));
	dbg_assert(__is_object(b));
	dbg_assert(itr_a->__cast == o_dlist_itr_cast);
	dbg_assert(itr_b->__cast == o_dlist_itr_cast);

	return itr_a->current == itr_b->current;
}

static int o_dlist_itr_compare_to(const_iterator itr, const_iterator other) {
	// TODO
	dbg_assert(false);
	return 0;
}
static hashcode o_dlist_itr_hashcode(const_iterator itr) {
	// TODO
	dbg_assert(false);
	return 0;
}

static unknown* o_dlist_itr_get_obj(const_iterator citr) {
	const struct o_dlist_itr* itr   = (const struct o_dlist_itr*)citr;
	const struct o_dlist_node* node = NULL;
	struct o_dlist* container = itr->container;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct o_dlist_node, link);

	/* return a internal reference as the iitr.h header said */
	return container->content_traits.__clone(node->reference, (pf_alloc)__global_default_alloc, __global_default_heap);
}

static void o_dlist_itr_set_obj(iterator citr, const unknown* n_ref) {
	struct o_dlist_itr* itr   = (struct o_dlist_itr*)citr;
	struct o_dlist_node* node = NULL;
	struct o_dlist* container = itr->container;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct o_dlist_node, link);
	
	/* first destroy the old reference */
	container->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, container->allocator);

	/* then clone the new reference */
	node->reference = 
		container->content_traits.__clone(n_ref, (pf_alloc)allocator_acquire, container->allocator);
}

static const unknown* o_dlist_itr_get_ref(const_iterator citr) {
	const struct o_dlist_itr* itr   = (const struct o_dlist_itr*)citr;
	const struct o_dlist_node* node = NULL;
	//struct o_dlist* container = itr->container;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct o_dlist_node, link);

	return node->reference;
}

static void o_dlist_itr_swap_ref(iterator citr, iterator other) {
	struct o_dlist_itr* itr_a = (struct o_dlist_itr*)citr;
	struct o_dlist_itr* itr_b = (struct o_dlist_itr*)other;
	struct o_dlist_node* node_a = container_of(itr_a->current, struct o_dlist_node, link);
	struct o_dlist_node* node_b = container_of(itr_b->current, struct o_dlist_node, link);
	unknown* temp;

	dbg_assert(itr_a->__cast == o_dlist_itr_cast);
	dbg_assert(itr_b->__cast == o_dlist_itr_cast);
	dbg_assert(itr_a->current != NULL && itr_b->current != NULL);
	dbg_assert(itr_a->container == itr_b->container);

	temp = node_a->reference;
	node_a->reference = node_b->reference;
	node_b->reference = temp;
}

static void o_dlist_itr_to_next(iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = itr->current->next;
}

static void o_dlist_itr_to_prev(iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	itr->current = itr->current->prev;
}

static struct iobject_vtable __olist_itr_iobject_vtable = {
	o_dlist_itr_destroy,      /* __destroy */
	o_dlist_itr_clone,        /* __clone   */
	o_dlist_itr_equals,       /* __equals  */
	o_dlist_itr_compare_to,   /* __compare_to */
	o_dlist_itr_hashcode
};

static struct itr_bidirectional_vtable __olist_itr_vtable = {
	o_dlist_itr_get_obj,      /* __get_obj */
	o_dlist_itr_get_ref,      /* __get_ref */
	o_dlist_itr_set_obj,      /* __set_obj */
	o_dlist_itr_swap_ref,     /* __swap_ref */
	o_dlist_itr_to_next,      /* __to_next */
	o_dlist_itr_to_prev       /* __to_prev */
};

static unknown* o_dlist_cast(unknown* x, unique_id intf_id) {
	struct o_dlist* o = (struct o_dlist*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IOBJECT_ID:
		return (unknown*)&o->__iftable[e_object];
	case ILIST_ID:
		return (unknown*)&o->__iftable[e_list];
	case IQUEUE_ID:
		return (unknown*)&o->__iftable[e_queue];
	case ISTACK_ID:
		return (unknown*)&o->__iftable[e_stack];
	default:
		return NULL;
	}

	return NULL;
}

static unknown* o_dlist_itr_cast(unknown* x, unique_id inf_id) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)x;

	dbg_assert(__is_object(itr));

	switch (inf_id) {
	case IOBJECT_ID:
		return (unknown*)&itr->__iftable[itr_interface_iobject];
	case ITR_REF_ID:
	case ITR_ACC_ID:
	case ITR_FWD_ID:
	case ITR_BID_ID:
		return (unknown*)&itr->__iftable[itr_interface_iterator];
	case ITR_RAC_ID:
		return NULL;
	default:
		return NULL;
	}

	return NULL;
}

_object* cntr_create_olist(unknown_traits* content_traits) {
	return olist_create(content_traits, __global_default_allocator);
}
_object* cntr_create_olist_a(unknown_traits* content_traits, allocator alc) {
	return olist_create(content_traits, alc);
}

static void o_dlist_itr_com_init(struct o_dlist_itr* itr, struct o_dlist* list);
_object* olist_create(unknown_traits* traits, allocator alc) {
	struct o_dlist* olist = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	olist = (struct o_dlist*)allocator_alloc(alc, sizeof(struct o_dlist));

	olist->__offset = olist;
	olist->__cast   = o_dlist_cast;
	
	olist->__iftable[e_object].__offset = (address)e_object;
	olist->__iftable[e_object].__vtable = &__olist_iobject_vtable;
	olist->__iftable[e_list].__offset   = (address)e_list;
	olist->__iftable[e_list].__vtable   = &__olist_ilist_vtable;
	olist->__iftable[e_queue].__offset  = (address)e_queue;
	olist->__iftable[e_queue].__vtable  = &__olist_iqueue_vtable;
	olist->__iftable[e_stack].__offset  = (address)e_stack;
	olist->__iftable[e_stack].__vtable  = &__olist_istack_vtable;

	list_init(&olist->sentinel);
	olist->size    = 0;

	olist->content_traits.__destroy    = traits->__destroy;
	olist->content_traits.__clone      = traits->__clone;
	olist->content_traits.__compare_to = traits->__compare_to;
	olist->content_traits.__equals     = traits->__equals;
	olist->content_traits.__hashcode   = traits->__hashcode;

	olist->allocator = alc;
	olist->allocator_join_ondispose = managed_allocator;

	o_dlist_itr_com_init(&olist->itr_begin, olist);
	o_dlist_itr_com_init(&olist->itr_end, olist);

	return (_object*)olist;
}

void olist_destroy(_object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;
	allocator alc = olist->allocator;
	bool join_alc = olist->allocator_join_ondispose;

	olist_clear(o);
	allocator_dealloc(olist->allocator, olist);

	if (join_alc) {
		allocator_join(alc);
	}
}
_object* olist_clone(const _object* o) {
	return NULL;
}
bool olist_equals(const _object* o, const _object* other) {
	return false;
}
int olist_compare_to(const _object* o, const _object* other) {
	return 0;
}
hashcode olist_hashcode(const _object* o) {
	return 0;
}

static void olistlink_dispose(struct listlink* link, void* context) {
	struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
	struct o_dlist* olist = (struct o_dlist*)context;

	/* first destroy the reference */
	olist->content_traits.__destroy(node->reference, (pf_dealloc)allocator_release, olist->allocator);

	/* delete the node it self */
	allocator_dealloc(olist->allocator, node);
}
void olist_clear(_object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	list_foreach_v(&olist->sentinel, olistlink_dispose, (void*)olist);

	list_init(&olist->sentinel);
	olist->size = 0;
}

struct olistlink_foreach_pack {
	pf_ref_process_v callback;
	void*            context;
};
static void olistlink_foreach_v(struct listlink* link, void* context) {
	struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
	struct olistlink_foreach_pack* pack = (struct olistlink_foreach_pack*)context;

	dbg_assert(pack->callback);
	pack->callback(node->reference, pack->context);
}
void olist_foreach(_object* o, pf_ref_process_v process, void* context) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct olistlink_foreach_pack pack = { process, context	};

	list_foreach_v(&olist->sentinel, olistlink_foreach_v, (void*)&pack);
}

int olist_size(const _object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;

	return olist->size;
}

bool olist_empty(const _object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;

	return olist->size == 0;
}

const unknown* olist_front(const _object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;
	struct o_dlist_node* n_node = NULL;

	if (olist->size == 0) {
		return NULL;
	}

	n_node = container_of(olist->sentinel.next, struct o_dlist_node, link);

	//return olist->content_traits.__clone(n_node->reference, __global_default_alloc, __global_default_heap);
	return n_node->reference;
}

const unknown* olist_back(const _object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;
	struct o_dlist_node* n_node = NULL;

	if (olist->size == 0) {
		return NULL;
	}

	n_node = container_of(olist->sentinel.prev, struct o_dlist_node, link);

	//return olist->content_traits.__clone(n_node->reference, __global_default_alloc, __global_default_heap);
	return n_node->reference;
}

void olist_add_front(_object* o, const unknown* __ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	n_node->reference = olist->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, olist->allocator);

	list_insert_front(&olist->sentinel, &n_node->link);
	olist->size ++;
}

void olist_add_back(_object* o, const unknown* __ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	n_node->reference = olist->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, olist->allocator);

	list_insert_back(&olist->sentinel, &n_node->link);
	olist->size ++;
}

void olist_remove_front(_object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	if (olist->size > 0) {
		struct listlink* link    = olist->sentinel.next;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		unknown* object_ref   = node->reference;

		dbg_assert(link != &olist->sentinel);

		list_unlink(link);
		allocator_dealloc(olist->allocator, node);

		olist->content_traits.__destroy(object_ref, (pf_dealloc)allocator_release, olist->allocator);

		olist->size --;

		return;
	}

	dbg_assert(olist->size == 0);
	dbg_assert(list_empty(&olist->sentinel));

	return;
}

void olist_remove_back(_object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	if (olist->size > 0) {
		struct listlink* link     = olist->sentinel.prev;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		unknown* object_ref       = node->reference;

		dbg_assert(link != &olist->sentinel);

		list_unlink(link);
		allocator_dealloc(olist->allocator, node);

		olist->content_traits.__destroy(object_ref, (pf_dealloc)allocator_release, olist->allocator);

		olist->size --;
		return;
	}

	dbg_assert(olist->size == 0);
	dbg_assert(list_empty(&olist->sentinel));

	return;
}

bool olist_contains(const _object* o, const unknown* __ref) {
	const struct o_dlist* olist  = (const struct o_dlist*)o;
	const struct listlink* link = olist->sentinel.next;

	while (link != &olist->sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (olist->content_traits.__equals(temp->reference, __ref)) {
			return true;
		}

		link = link->next;
	}

	return false;
}

bool olist_remove(_object* o, const unknown* __ref) {
	struct o_dlist* olist  = (struct o_dlist*)o;
	struct listlink* link = olist->sentinel.next;

	while (link != &olist->sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (olist->content_traits.__equals(temp->reference, __ref)) {
			break;
		}

		link = link->next;
	}

	if (link != &olist->sentinel) {
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		unknown* object_ref = node->reference;

		list_unlink(link);
		allocator_dealloc(olist->allocator, node);
		olist->content_traits.__destroy(object_ref, (pf_dealloc)allocator_release, olist->allocator);

		olist->size --;
		return true;
	}
	return false;
}


/* initialize all part of an iterator except the __current position */
static void o_dlist_itr_com_init(struct o_dlist_itr* itr, struct o_dlist* list) {
	itr->__offset = itr;
	itr->__cast   = o_dlist_itr_cast;

	itr->__iftable[itr_interface_iobject].__offset = (address)itr_interface_iobject;
	itr->__iftable[itr_interface_iobject].__vtable = (unknown*)&__olist_itr_iobject_vtable;
	itr->__iftable[itr_interface_iterator].__offset = (address)itr_interface_iterator;
	itr->__iftable[itr_interface_iterator].__vtable = (unknown*)&__olist_itr_vtable;

	itr->container = list;
	itr->allocator = list->allocator;
	/* itr->__current = NULL; */
}

const_iterator olist_itr_begin(const _object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	/* if the list is empty, we just return the sentinel node */
	olist->itr_begin.current = olist->sentinel.next;

	return (const_iterator)&olist->itr_begin;
}

const_iterator olist_itr_end(const _object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	olist->itr_end.current = &olist->sentinel;

	return (const_iterator)&olist->itr_end;
}

iterator olist_itr_create(const _object* o, itr_pos pos) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* n_itr = (struct o_dlist_itr*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_itr));

	o_dlist_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		/* if the list is empty, we just return the sentinel node */
		n_itr->current = olist->sentinel.next;
	}
	else if (pos == itr_end) {
		/* the end iterator is the sentinel node, since 
		 * (begin, end) represents [begin, end) */
		n_itr->current = &olist->sentinel;
	}
	else {
		dbg_assert(false);
	}

	return (iterator)n_itr;
}

void olist_itr_assign(const _object* o, iterator __itr, itr_pos pos) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* itr = (struct o_dlist_itr*)__itr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->container == olist);
	dbg_assert(itr->current != NULL);

	o_dlist_itr_com_init(itr, olist);

	if (pos == itr_begin) {
		/* if the list is empty, we just return the sentinel node */
		itr->current = olist->sentinel.next;
	}
	else if (pos == itr_end) {
		/* the end iterator is the sentinel node, since 
		 * (begin, end) represents [begin, end) */
		itr->current = &olist->sentinel;
	}
	else {
		dbg_assert(false);
	}

	return;
}

void olist_itr_find(const _object* o, iterator itr, const unknown* __ref) {
	struct o_dlist* olist    = (struct o_dlist*)o;
	struct o_dlist_itr* oitr = (struct o_dlist_itr*)itr;
	struct listlink* link   = olist->sentinel.next;

	/* make sure the type information is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (void*)&__olist_itr_vtable);

	dbg_assert(oitr->container == olist);

	while (link != &olist->sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (olist->content_traits.__equals(temp->reference, __ref)) {
			break;
		}

		link = link->next;
	}

	oitr->current = link;
}

void olist_itr_remove(_object* o, iterator itr) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* oitr  = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node = container_of(oitr->current, struct o_dlist_node, link);
	unknown* obj_ref           = node->reference;

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->container == olist);
	dbg_assert(oitr->current != NULL);

	list_unlink(&node->link);
	
	allocator_dealloc(olist->allocator, node);
	olist->content_traits.__destroy(obj_ref, (pf_dealloc)allocator_release, olist->allocator);

	olist->size --;

	/* invalidate the iterator */
	oitr->current = NULL;

	return;
}

void olist_insert_before(_object* o, iterator itr, const unknown* __ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node   = container_of(oitr->current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->container == olist);
	dbg_assert(oitr->current != NULL);

	n_node->reference = olist->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, olist->allocator);
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	olist->size ++;
}

void olist_insert_after(_object* o, iterator itr, const unknown* __ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node   = container_of(oitr->current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->container == olist);
	dbg_assert(oitr->current != NULL);

	n_node->reference = olist->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, olist->allocator);
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	olist->size ++;
}
