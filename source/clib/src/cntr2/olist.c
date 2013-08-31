#include <string.h>

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
	allocator                     __allocator;

	struct list_link*             __current;
};

struct o_dlist {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject         __iftable[e_l_count];

	struct list_link              __sentinel;
	int                           __size;

	/* methods to manage the inner memory use by the container */
	allocator                     __allocator;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_dispose                    __dispose;

	struct o_dlist_itr            __itr_begin;
	struct o_dlist_itr            __itr_end;
};

static object*  o_dlist_create          ();
static object*  o_dlist_create_v        (allocator alc, pf_dispose dispose);
static void     o_dlist_destroy         (object* o);
static void     o_dlist_clear           (object* o);
static int      o_dlist_size            (const object* o);
static bool     o_dlist_empty           (const object* o);
static const void* o_dlist_front        (const object* o);
static const void* o_dlist_back         (const object* o);
static void     o_dlist_add_front       (object* o, void* __ref);
static void     o_dlist_add_back        (object* o, void* __ref);
static void*    o_dlist_remove_front    (object* o);
static void*    o_dlist_remove_back     (object* o);
static bool     o_dlist_contains        (const object* o, void* __ref);
static bool     o_dlist_remove          (object* o, void* __ref);

static iterator o_dlist_itr_create      (const object* o, itr_pos pos);
static void     o_dlist_itr_assign      (const object* o, iterator itr, itr_pos pos);
static void     o_dlist_itr_find        (const object* o, iterator itr, void* __ref);
static void*    o_dlist_itr_remove      (object* o, iterator itr);
static void     o_dlist_insert_before   (object* o, iterator itr, void* n_ref);
static void     o_dlist_insert_after    (object* o, iterator itr, void* n_ref);
static const_iterator o_dlist_itr_begin (const object* o);
static const_iterator o_dlist_itr_end   (const object* o);

/* factory method, the only public function in this file */
object* cntr_create_olist() {
	return o_dlist_create();
}
object* cntr_create_olist_v(allocator alc, pf_dispose dispose) {
	return o_dlist_create_v(alc, dispose);
}

static struct ilist_vtable __ilist_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
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
	allocator_dealloc(itr->__allocator, itr);
}

static iterator o_dlist_itr_clone(const_iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;
	struct o_dlist_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == o_dlist_itr_cast);

	/* destroy itself */
	n_itr = (struct o_dlist_itr*)allocator_alloc(itr->__allocator, sizeof(struct o_dlist_itr));

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

	return itr_a->__current == itr_b->__current;
}

static const void* o_dlist_itr_get_ref(const_iterator citr) {
	const struct o_dlist_itr* itr   = (const struct o_dlist_itr*)citr;
	const struct o_dlist_node* node = NULL;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = container_of(itr->__current, struct o_dlist_node, link);
	return node->reference;
}

static void o_dlist_itr_set_ref(iterator citr, const void* n_ref) {
	struct o_dlist_itr* itr   = (struct o_dlist_itr*)citr;
	struct o_dlist_node* node = NULL;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = container_of(itr->__current, struct o_dlist_node, link);
	node->reference = n_ref;
}

static void o_dlist_itr_to_next(iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = itr->__current->next;
}

static void o_dlist_itr_to_prev(iterator citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = itr->__current->prev;
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

static object* o_dlist_create() {
	return o_dlist_create_v(default_allocator, NULL);
}

static void o_dlist_itr_com_init(struct o_dlist_itr* itr, struct o_dlist* list);
static object* o_dlist_create_v(allocator alc, pf_dispose dispose) {
	struct o_dlist* olist = NULL;

	dbg_assert(alc != NULL);

	olist = (struct o_dlist*)allocator_alloc(alc, sizeof(struct o_dlist));

	olist->__offset = olist;
	olist->__cast   = o_dlist_cast;
	
	olist->__iftable[e_list].__offset = (address)e_list;
	olist->__iftable[e_list].__vtable = &__ilist_vtable;
	olist->__iftable[e_queue].__offset = (address)e_queue;
	olist->__iftable[e_queue].__vtable = &__iqueue_vtable;
	olist->__iftable[e_stack].__offset = (address)e_stack;
	olist->__iftable[e_stack].__vtable = &__istack_vtable;

	list_init(&olist->__sentinel);
	olist->__size    = 0;

	olist->__allocator = alc;

	olist->__dispose = dispose;

	o_dlist_itr_com_init(&olist->__itr_begin, olist);
	o_dlist_itr_com_init(&olist->__itr_end, olist);

	return (object*)olist;
}

static void per_link_dispose(struct list_link* link, void* param) {
	struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
	struct o_dlist* olist     = (struct o_dlist*)param;

	/* first dispose the reference */
	if (olist->__dispose) {
		olist->__dispose((void*)node->reference);
	}
	
	/* delete the node it self */
	allocator_dealloc(olist->__allocator, node);
}

static void o_dlist_destroy(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	o_dlist_clear(o);
	allocator_dealloc(olist->__allocator, olist);
}

static void o_dlist_clear(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	list_foreach_v(&olist->__sentinel, per_link_dispose, (void*)olist);

	list_init(&olist->__sentinel);
	olist->__size = 0;
}

static int o_dlist_size(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;

	return olist->__size;
}

static bool o_dlist_empty(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;

	return olist->__size == 0;
}

static const void* o_dlist_front(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;
	struct o_dlist_node* n_node = NULL;

	if (olist->__size == 0) {
		return NULL;
	}

	n_node = container_of(olist->__sentinel.next, struct o_dlist_node, link);

	return n_node->reference;
}

static const void* o_dlist_back(const object* o) {
	const struct o_dlist* olist = (const struct o_dlist*)o;
	struct o_dlist_node* n_node = NULL;

	if (olist->__size == 0) {
		return NULL;
	}

	n_node = container_of(olist->__sentinel.prev, struct o_dlist_node, link);

	return n_node->reference;
}

static void o_dlist_add_front(object* o, void* ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_dlist_node));

	n_node->reference = ref;

	list_insert_front(&olist->__sentinel, &n_node->link);
	olist->__size ++;
}

static void o_dlist_add_back(object* o, void* ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_dlist_node));

	n_node->reference = ref;

	list_insert_back(&olist->__sentinel, &n_node->link);
	olist->__size ++;
}

static void* o_dlist_remove_front(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	if (olist->__size > 0) {
		struct list_link* link    = olist->__sentinel.next;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		const void*  object_ref   = node->reference;

		dbg_assert(link != &olist->__sentinel);

		list_unlink(link);
		allocator_dealloc(olist->__allocator, node);

		olist->__size --;

		/* const cast */
		return (void*)object_ref;
	}

	dbg_assert(olist->__size == 0);
	dbg_assert(list_empty(&olist->__sentinel));

	return NULL;
}

static void* o_dlist_remove_back(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	if (olist->__size > 0) {
		struct list_link* link    = olist->__sentinel.prev;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		const void*  object_ref   = node->reference;

		dbg_assert(link != &olist->__sentinel);

		list_unlink(link);
		allocator_dealloc(olist->__allocator, node);

		olist->__size --;

		return (void*)object_ref;
	}

	dbg_assert(olist->__size == 0);
	dbg_assert(list_empty(&olist->__sentinel));

	return NULL;
}

static bool o_dlist_contains(const object* o, void* __ref) {
	const struct o_dlist* olist     = (const struct o_dlist*)o;
	const struct list_link* link    = olist->__sentinel.next;

	while (link != &olist->__sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (temp->reference == __ref) {
			return true;
		}

		link = link->next;
	}

	return false;
}

static bool o_dlist_remove(object* o, void* __ref) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct list_link* link    = olist->__sentinel.next;

	while (link != &olist->__sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (temp->reference == __ref) {
			break;
		}

		link = link->next;
	}

	if (link != &olist->__sentinel) {
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);

		list_unlink(link);
		allocator_dealloc(olist->__allocator, node);

		olist->__size --;
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

	itr->__allocator = list->__allocator;
	/* itr->__current = NULL; */
}

static const_iterator o_dlist_itr_begin(const object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	/* if the list is empty, we just return the sentinel node */
	olist->__itr_begin.__current = olist->__sentinel.next;

	return (const_iterator)&olist->__itr_begin;
}

static const_iterator o_dlist_itr_end(const object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	olist->__itr_end.__current = &olist->__sentinel;

	return (const_iterator)&olist->__itr_end;
}

static iterator o_dlist_itr_create(const object* o, itr_pos pos) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* n_itr = (struct o_dlist_itr*)
		allocator_alloc(olist->__allocator, sizeof(struct o_dlist_itr));

	o_dlist_itr_com_init(n_itr, olist);

	if (pos == itr_begin) {
		/* if the list is empty, we just return the sentinel node */
		n_itr->__current = olist->__sentinel.next;
	}
	else if (pos == itr_end) {
		/* the end iterator is the sentinel node, since 
		 * (begin, end) represents [begin, end) */
		n_itr->__current = &olist->__sentinel;
	}
	else {
		dbg_assert(false);
	}

	return (iterator)n_itr;
}

static void o_dlist_itr_assign(const object* o, iterator __itr, itr_pos pos) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* itr = (struct o_dlist_itr*)__itr;

	o_dlist_itr_com_init(itr, olist);

	if (pos == itr_begin) {
		/* if the list is empty, we just return the sentinel node */
		itr->__current = olist->__sentinel.next;
	}
	else if (pos == itr_end) {
		/* the end iterator is the sentinel node, since 
		 * (begin, end) represents [begin, end) */
		itr->__current = &olist->__sentinel;
	}
	else {
		dbg_assert(false);
	}

	return;
}

static void o_dlist_itr_find(const object* o, iterator itr, void* __ref) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* oitr = (struct o_dlist_itr*)itr;
	struct list_link* link    = olist->__sentinel.next;
	struct o_dlist_node* node = NULL;

	/* make sure the type information is right */
	dbg_assert(itr->__iftable[0].__offset == (address)0);
	dbg_assert(itr->__iftable[0].__vtable == (unknown)&__itr_vtable);

	while (link != &olist->__sentinel) {
		struct o_dlist_node* temp = container_of(link, struct o_dlist_node, link);

		if (temp->reference == __ref) {
			node = temp;
			break;
		}

		link = link->next;
	}

	if (node != NULL) {
		oitr->__current = &(node->link);
	}
	else {
		oitr->__current = &olist->__sentinel;
	}
}

static void* o_dlist_itr_remove(object* o, iterator itr) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* oitr  = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node = container_of(oitr->__current, struct o_dlist_node, link);
	const void* obj_ref       = node->reference;

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	list_unlink(&node->link);
	//olist->__dealloc(node);
	allocator_dealloc(olist->__allocator, node);

	olist->__size --;

	return (void*)obj_ref;
}

static void o_dlist_insert_before(object* o, iterator itr, void* n_ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node   = container_of(oitr->__current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	olist->__size ++;
}

static void o_dlist_insert_after(object* o, iterator itr, void* n_ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)itr;
	struct o_dlist_node* node   = container_of(oitr->__current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	olist->__size ++;
}
