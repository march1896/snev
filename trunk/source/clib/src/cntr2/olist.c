#include <string.h>

#include "olist.h"
#include "ilist.h"
#include "iqueue.h"
#include "istack.h"
#include "iitr.h"
#include "ifactory.h"
#include "oallocator.h"
#include "util/list_link.h"

/* this module defines double linked list(with sentinel) container, it implements 
 * ilist, iqueue, istack 
 * and we do not expose the object method currently, that is you have to use one of the interface 
 * above, but not the list directly. */

enum list_interfaces {
	e_list,
	e_queue,
	e_stack,
	e_l_count
};

struct o_dlist_node {
	struct list_link              link;

	const void*                   reference;
};

struct o_dlist_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	iobject                       __iftable[1];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     allocator;

	struct list_link*             current;
};

struct o_dlist {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_l_count];

	struct list_link              sentinel;
	int                           size;

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;

	struct o_dlist_itr            itr_begin;
	struct o_dlist_itr            itr_end;
};

/* factory method, the only public function in this file */
object* cntr_create_olist() {
	return o_dlist_create();
}
object* cntr_create_olist_v(allocator alc) {
	return o_dlist_create_v(alc);
}

static struct ilist_vtable __ilist_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_clear_v,          /* __clear_v */
	o_dlist_size,             /* __size */
	o_dlist_empty,            /* __empty */
	o_dlist_front,            /* __front */
	o_dlist_back,             /* __back */
	o_dlist_add_front,        /* __add_front */
	o_dlist_add_back,         /* __add_back */
	o_dlist_remove_front,     /* __remove_front */
	o_dlist_remove_back,      /* __remove_back */
	o_dlist_contains,         /* __contains */
	o_dlist_remove,           /* __remove  */

	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end,          /* __itr_end */
	o_dlist_itr_create,       /* __itr_create */
	o_dlist_itr_assign,       /* __itr_assign */
	o_dlist_itr_find,         /* __itr_find */
	o_dlist_itr_remove,       /* __itr_remove */
	o_dlist_insert_before,    /* __insert_before */
	o_dlist_insert_after      /* __insert_after */
};

static struct iqueue_vtable __iqueue_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_clear_v,          /* __clear_v */
	o_dlist_size,             /* __size */
	o_dlist_empty,            /* __empty */
	o_dlist_front,            /* __front */
	o_dlist_back,             /* __back */
	o_dlist_add_back,         /* __push */
	o_dlist_remove_front,     /* __pop */
	o_dlist_itr_create,       /* __itr_create */
	o_dlist_itr_assign,       /* __itr_assign */
	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end           /* __itr_end */
};

static struct istack_vtable __istack_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_clear_v,          /* __clear_v */
	o_dlist_size,             /* __size */
	o_dlist_empty,            /* __empty */
	o_dlist_back,             /* __top */
	o_dlist_add_back,         /* __push */
	o_dlist_remove_back,      /* __pop */
	o_dlist_itr_create,       /* __itr_create */
	o_dlist_itr_assign,       /* __itr_assign */
	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end           /* __itr_end */
};

static unknown o_dlist_itr_cast(unknown x, unique_id inf_id);
static unknown o_dlist_cast(unknown x, unique_id intf_id);

static void o_dlist_itr_destroy(iterator citr);
static iterator o_dlist_itr_clone(const_iterator citr);
static bool o_dlist_itr_equals(const_iterator a, const_iterator b);
static const void* o_dlist_itr_get_ref(const_iterator citr);
static void o_dlist_itr_set_ref(iterator citr, const void* n_ref);
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
	/* TODO: this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool o_dlist_itr_equals(const_iterator a, const_iterator b) {
	const struct o_dlist_itr* itr_a = (const struct o_dlist_itr*)a;
	const struct o_dlist_itr* itr_b = (const struct o_dlist_itr*)b;

	dbg_assert(__is_object((unknown)a));
	dbg_assert(__is_object((unknown)b));
	dbg_assert(itr_a->__cast == o_dlist_itr_cast);
	dbg_assert(itr_b->__cast == o_dlist_itr_cast);

	return itr_a->current == itr_b->current;
}

static const void* o_dlist_itr_get_ref(const_iterator citr) {
	const struct o_dlist_itr* itr   = (const struct o_dlist_itr*)citr;
	const struct o_dlist_node* node = NULL;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct o_dlist_node, link);
	return node->reference;
}

static void o_dlist_itr_set_ref(iterator citr, const void* n_ref) {
	struct o_dlist_itr* itr   = (struct o_dlist_itr*)citr;
	struct o_dlist_node* node = NULL;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->current != NULL);

	node = container_of(itr->current, struct o_dlist_node, link);
	node->reference = n_ref;
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

static struct itr_bidirectional_vtable __itr_vtable = {
	o_dlist_itr_destroy,      /* __destroy */
	o_dlist_itr_clone,        /* __clone   */
	o_dlist_itr_equals,       /* __equals  */
	o_dlist_itr_get_ref,      /* __get_ref */
	o_dlist_itr_set_ref,      /* __set_ref */
	o_dlist_itr_to_next,      /* __to_next */
	o_dlist_itr_to_prev       /* __to_prev */
};

static unknown o_dlist_cast(unknown x, unique_id intf_id) {
	struct o_dlist* o = (struct o_dlist*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case ILIST_ID:
		return (unknown)&o->__iftable[e_list];
	case IQUEUE_ID:
		return (unknown)&o->__iftable[e_queue];
	case ISTACK_ID:
		return (unknown)&o->__iftable[e_stack];
	default:
		return NULL;
	}

	return NULL;
}

static unknown o_dlist_itr_cast(unknown x, unique_id inf_id) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)x;

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

object* o_dlist_create() {
	return o_dlist_create_v(__global_default_allocator);
}

static void o_dlist_itr_com_init(struct o_dlist_itr* itr, struct o_dlist* list);
object* o_dlist_create_v(allocator alc) {
	struct o_dlist* olist = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	olist = (struct o_dlist*)allocator_alloc(alc, sizeof(struct o_dlist));

	olist->__offset = olist;
	olist->__cast   = o_dlist_cast;
	
	olist->__iftable[e_list].__offset = (address)e_list;
	olist->__iftable[e_list].__vtable = &__ilist_vtable;
	olist->__iftable[e_queue].__offset = (address)e_queue;
	olist->__iftable[e_queue].__vtable = &__iqueue_vtable;
	olist->__iftable[e_stack].__offset = (address)e_stack;
	olist->__iftable[e_stack].__vtable = &__istack_vtable;

	list_init(&olist->sentinel);
	olist->size    = 0;

	olist->allocator = alc;
	olist->allocator_join_ondispose = managed_allocator;

	o_dlist_itr_com_init(&olist->itr_begin, olist);
	o_dlist_itr_com_init(&olist->itr_end, olist);

	return (object*)olist;
}

void o_dlist_destroy(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;
	allocator alc = olist->allocator;
	bool join_alc = olist->allocator_join_ondispose;

	o_dlist_clear(o);
	allocator_dealloc(olist->allocator, olist);

	if (join_alc) {
		allocator_join(alc);
	}
}

static void olistlink_dispose(struct list_link* link, void* context) {
	struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
	struct o_dlist* olist = (struct o_dlist*)context;

	/* delete the node it self */
	allocator_dealloc(olist->allocator, node);
}
void o_dlist_clear(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	list_foreach_v(&olist->sentinel, olistlink_dispose, (void*)olist);

	list_init(&olist->sentinel);
	olist->size = 0;
}

struct olistlink_clear_v {
	pf_ref_dispose_v callback;
	void*            context;
	struct o_dlist*  container;
};
static void olistlink_dispose_v(struct list_link* link, void* context) {
	struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
	struct olistlink_clear_v* params = (struct olistlink_clear_v*)context;

	/* first dispose the reference */
	if (params->callback) {
		params->callback((void*)node->reference, params->container);
	}

	/* delete the node it self */
	dbg_assert(params && params->container && params->container->allocator);
	allocator_dealloc(params->container->allocator, node);
}
void o_dlist_clear_v(object* o, pf_ref_dispose_v dispose, void* context) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct olistlink_clear_v clear_params = {
		dispose, context, olist
	};

	list_foreach_v(&olist->sentinel, olistlink_dispose_v, (void*)&clear_params);

	list_init(&olist->sentinel);
	olist->size = 0;
}

int o_dlist_size(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;

	return olist->size;
}

bool o_dlist_empty(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;

	return olist->size == 0;
}

const void* o_dlist_front(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;
	struct o_dlist_node* n_node = NULL;

	if (olist->size == 0) {
		return NULL;
	}

	n_node = container_of(olist->sentinel.next, struct o_dlist_node, link);

	return n_node->reference;
}

const void* o_dlist_back(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;
	struct o_dlist_node* n_node = NULL;

	if (olist->size == 0) {
		return NULL;
	}

	n_node = container_of(olist->sentinel.prev, struct o_dlist_node, link);

	return n_node->reference;
}

void o_dlist_add_front(object* o, const void* ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	n_node->reference = ref;

	list_insert_front(&olist->sentinel, &n_node->link);
	olist->size ++;
}

void o_dlist_add_back(object* o, const void* ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	n_node->reference = ref;

	list_insert_back(&olist->sentinel, &n_node->link);
	olist->size ++;
}

void* o_dlist_remove_front(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	if (olist->size > 0) {
		struct list_link* link    = olist->sentinel.next;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		const void*  object_ref   = node->reference;

		dbg_assert(link != &olist->sentinel);

		list_unlink(link);
		allocator_dealloc(olist->allocator, node);

		olist->size --;

		/* const cast */
		return (void*)object_ref;
	}

	dbg_assert(olist->size == 0);
	dbg_assert(list_empty(&olist->sentinel));

	return NULL;
}

void* o_dlist_remove_back(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	if (olist->size > 0) {
		struct list_link* link    = olist->sentinel.prev;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		const void*  object_ref   = node->reference;

		dbg_assert(link != &olist->sentinel);

		list_unlink(link);
		allocator_dealloc(olist->allocator, node);

		olist->size --;

		return (void*)object_ref;
	}

	dbg_assert(olist->size == 0);
	dbg_assert(list_empty(&olist->sentinel));

	return NULL;
}

bool o_dlist_contains(const object* o, const void* __ref) {
	const struct o_dlist* olist     = (const struct o_dlist*)o;
	const struct list_link* link    = olist->sentinel.next;

	while (link != &olist->sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (temp->reference == __ref) {
			return true;
		}

		link = link->next;
	}

	return false;
}

bool o_dlist_remove(object* o, void* __ref) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct list_link* link    = olist->sentinel.next;

	while (link != &olist->sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (temp->reference == __ref) {
			break;
		}

		link = link->next;
	}

	if (link != &olist->sentinel) {
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);

		list_unlink(link);
		allocator_dealloc(olist->allocator, node);

		olist->size --;
		return true;
	}
	return false;
}


/* initialize all part of an iterator except the __current position */
static void o_dlist_itr_com_init(struct o_dlist_itr* itr, struct o_dlist* list) {
	itr->__offset = itr;
	itr->__cast   = o_dlist_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->allocator = list->allocator;
	/* itr->__current = NULL; */
}

const_iterator o_dlist_itr_begin(const object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	/* if the list is empty, we just return the sentinel node */
	olist->itr_begin.current = olist->sentinel.next;

	return (const_iterator)&olist->itr_begin;
}

const_iterator o_dlist_itr_end(const object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	olist->itr_end.current = &olist->sentinel;

	return (const_iterator)&olist->itr_end;
}

iterator o_dlist_itr_create(const object* o, itr_pos pos) {
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

void o_dlist_itr_assign(const object* o, iterator __itr, itr_pos pos) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* itr = (struct o_dlist_itr*)__itr;

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

void o_dlist_itr_find(const object* o, iterator itr, void* __ref) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* oitr = (struct o_dlist_itr*)itr;
	struct list_link* link    = olist->sentinel.next;
	struct o_dlist_node* node = NULL;

	/* make sure the type information is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	while (link != &olist->sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (temp->reference == __ref) {
			node = temp;
			break;
		}

		link = link->next;
	}

	if (node != NULL) {
		oitr->current = &(node->link);
	}
	else {
		oitr->current = &olist->sentinel;
	}
}

void* o_dlist_itr_remove(object* o, iterator itr) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* oitr  = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node = container_of(oitr->current, struct o_dlist_node, link);
	const void* obj_ref       = node->reference;

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->current != NULL);

	list_unlink(&node->link);
	//olist->__dealloc(node);
	allocator_dealloc(olist->allocator, node);

	olist->size --;

	return (void*)obj_ref;
}

void o_dlist_insert_before(object* o, iterator itr, void* n_ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node   = container_of(oitr->current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->current != NULL);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	olist->size ++;
}

void o_dlist_insert_after(object* o, iterator itr, void* n_ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node   = container_of(oitr->current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->allocator, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->current != NULL);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	olist->size ++;
}
