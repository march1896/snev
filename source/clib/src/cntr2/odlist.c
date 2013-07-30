#include <ilist.h>
#include <iqueue.h>
#include <istack.h>
#include <iset.h>
#include <iitr.h>

#include <imemmgr.h>

#include <util/list_link.h>

enum list_interfaces {
	e_list,
	e_queue,
	e_stack,
	e_set,
	e_l_count
};

struct o_dlist_node {
	struct list_link              link;

	void*                         reference;
};

struct o_dlist {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	//struct r_dlist                __cntr;

	struct list_link              __thelist;
	int                           __size;

	/* methods to manage the inner memory use by the container */
	void*                         __heap;
	pf_alloc                      __alloc;
	pf_dealloc                    __dealloc;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_copy                       __copy;
	pf_dispose                    __dispose;
};

static object*  o_dlist_create          ();
static object*  o_dlist_create_v        (void* heap, pf_alloc alloc, pf_dealloc dealloc, pf_copy copy, pf_dispose dispose);
static void     o_dlist_destroy         (object* o);
static void     o_dlist_clear           (object* o);
static int      o_dlist_size            (object* o);
static void     o_dlist_add_front       (object* o, void* ref);
static void     o_dlist_add_back        (object* o, void* ref);
static void*    o_dlist_remove_front    (object* o);
static void*    o_dlist_remove_back     (object* o);
static object*  o_dlist_itr_find        (object* o, void* __ref);
static void*    o_dlist_remove          (object* o, iobject* itr);
static void     o_dlist_insert_before   (object* o, iobject* itr, void* n_ref);
static void     o_dlist_insert_after    (object* o, iobject* itr, void* n_ref);
static object*  o_dlist_itr_begin       (object* o);
static object*  o_dlist_itr_end         (object* o);

/* factory method, the only public function in this file */
object* create_dblinked_list() {
	return o_dlist_create();
}
object* create_dblinked_list_v(void* heap, pf_alloc alloc, pf_dealloc dealloc, pf_copy copy, pf_dispose dispose) {
	return o_dlist_create_v(heap, alloc, dealloc, copy, dispose);
}

static struct ilist_vtable __ilist_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_size,             /* __size */
	o_dlist_add_front,        /* __add_front */
	o_dlist_add_back,         /* __add_back */
	o_dlist_remove_front,     /* __remove_front */
	o_dlist_remove_back,      /* __remove_back */
	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end,          /* __itr_end */
	o_dlist_itr_find,         /* __itr_find */
	o_dlist_remove,           /* __remove */
	o_dlist_insert_before,    /* __insert_before */
	o_dlist_insert_after      /* __insert_after */
};

static struct iqueue_vtable __iqueue_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_size,             /* __size */
	o_dlist_add_back,         /* __push */
	o_dlist_remove_front,     /* __pop */
	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end           /* __itr_end */
};

static struct istack_vtable __istack_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_size,             /* __size */
	o_dlist_add_back,         /* __push */
	o_dlist_remove_back,      /* __pop */
	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end           /* __itr_end */
};

static struct iset_vtable __iset_vtable = {
	o_dlist_destroy,          /* __destroy */
	o_dlist_clear,            /* __clear */
	o_dlist_size,             /* __size */
	o_dlist_add_back,         /* __insert */
	o_dlist_itr_find,         /* __itr_find */
	o_dlist_remove,           /* __remove */
	o_dlist_itr_begin,        /* __itr_begin */
	o_dlist_itr_end           /* __itr_end */
};

typedef struct o_dlist_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	struct base_interface         __iftable[1];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	void*                         __heap;
	pf_dealloc                    __dealloc;

	struct list_link*             __current;
};


static unknown o_dlist_itr_cast(unknown x, unique_id inf_id);
static unknown o_dlist_cast(unknown x, unique_id intf_id);

static void o_dlist_itr_destroy(object* citr);
static bool o_dlist_itr_equals(object* a, object* b);
static void* o_dlist_itr_get_ref(object* citr);
static void o_dlist_itr_set_ref(object* citr, void* n_ref);
static void o_dlist_itr_to_next(object* citr);
static void o_dlist_itr_to_prev(object* citr);

static void o_dlist_itr_destroy(object* citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == o_dlist_itr_cast);

	/* destroy itself */
	//itr->__dealloc(itr);
	dealloc(itr->__dealloc, itr->__heap, itr);
}

static bool o_dlist_itr_equals(object* a, object* b) {
	struct o_dlist_itr* itr_a = (struct o_dlist_itr*)a;
	struct o_dlist_itr* itr_b = (struct o_dlist_itr*)b;

	dbg_assert(__is_object(a));
	dbg_assert(__is_object(b));
	dbg_assert(itr_a->__cast == o_dlist_itr_cast);
	dbg_assert(itr_b->__cast == o_dlist_itr_cast);

	return itr_a->__current == itr_b->__current;
}

static void* o_dlist_itr_get_ref(object* citr) {
	struct o_dlist_itr* itr   = (struct o_dlist_itr*)citr;
	struct o_dlist_node* node = NULL;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = (struct o_dlist_node*)container_of(itr->__current, struct o_dlist_node, link);
	return node->reference;
}

static void o_dlist_itr_set_ref(object* citr, void* n_ref) {
	struct o_dlist_itr* itr   = (struct o_dlist_itr*)citr;
	struct o_dlist_node* node = NULL;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = (struct o_dlist_node*)container_of(itr->__current, struct o_dlist_node, link);
	node->reference = n_ref;
}

static void o_dlist_itr_to_next(object* citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = itr->__current->next;
}

static void o_dlist_itr_to_prev(object* citr) {
	struct o_dlist_itr* itr = (struct o_dlist_itr*)citr;

	dbg_assert(itr->__cast == o_dlist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = itr->__current->prev;
}

static struct itr_bidirectional_vtable __itr_vtable = {
	o_dlist_itr_destroy,      /* __destroy */
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
	case ISET_ID:
		return (unknown)&o->__iftable[e_set];
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
	return o_dlist_create_v(NULL, NULL, NULL, NULL, NULL);
}

static object* o_dlist_create_v(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc, pf_copy copy, pf_dispose dispose) {
	struct o_dlist* olist = NULL;

	if (__heap == NULL) {
		// use the default heap.
		dbg_assert(__alloc == NULL && __dealloc == NULL);
		__alloc = cntr_default_alloc;
		__dealloc = cntr_default_dealloc;
	}

	olist = (struct o_dlist*)alloc(__alloc, __heap, sizeof(struct o_dlist));

	olist->__offset = olist;
	olist->__cast   = o_dlist_cast;
	
	olist->__iftable[e_list].__offset = (address)e_list;
	olist->__iftable[e_list].__vtable = &__ilist_vtable;
	olist->__iftable[e_queue].__offset = (address)e_queue;
	olist->__iftable[e_queue].__vtable = &__iqueue_vtable;
	olist->__iftable[e_stack].__offset = (address)e_stack;
	olist->__iftable[e_stack].__vtable = &__istack_vtable;
	olist->__iftable[e_set].__offset = (address)e_set;
	olist->__iftable[e_set].__vtable = &__iset_vtable;

	//r_dlist_init(&olist->__cntr, __alloc, __dealloc);
	list_init(&olist->__thelist);

	olist->__heap    = __heap;
	olist->__alloc   = __alloc;
	olist->__dealloc = __dealloc;

	olist->__size    = 0;

	olist->__copy    = copy;
	olist->__dispose = dispose;

	return (object*)olist;
}

static void per_link_dispose(struct list_link* link, void* param) {
	struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
	struct o_dlist* olist     = (struct o_dlist*)param;

	/* first dispose the reference */
	if (olist->__dispose) {
		olist->__dispose(node->reference);
	}
	
	/* delete the node it self */
	//olist->__dealloc(node);
	dealloc(olist->__dealloc, olist->__heap, node);
}

static void o_dlist_destroy(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	o_dlist_clear(o);
	dealloc(olist->__dealloc, olist->__heap, olist);
}

static void o_dlist_clear(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	list_foreach_v(&olist->__thelist, per_link_dispose, (void*)olist);

	list_init(&olist->__thelist);
	olist->__size = 0;
}

static int o_dlist_size(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	//return r_dlist_size(&olist->__cntr);
	return olist->__size;
}

static void o_dlist_add_front(object* o, void* ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	//r_dlist_add_front(&olist->__cntr, ref);
	//
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_node));

	n_node->reference = ref;

	list_insert_front(&olist->__thelist, &n_node->link);
	olist->__size ++;
}

static void o_dlist_add_back(object* o, void* ref) {
	struct o_dlist* olist = (struct o_dlist*)o;

	//r_dlist_add_back(&olist->__cntr, ref);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_node));

	n_node->reference = ref;

	list_insert_back(&olist->__thelist, &n_node->link);
	olist->__size ++;
}

static void* o_dlist_remove_front(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	//return r_dlist_remove_front(&olist->__cntr);
	if (olist->__size > 0) {
		struct list_link* link    = olist->__thelist.next;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		void*  object_ref         = node->reference;

		dbg_assert(link != &olist->__thelist);

		list_unlink(link);
		//olist->__dealloc(node);
		dealloc(olist->__dealloc, olist->__heap, node);

		olist->__size --;

		return object_ref;
	}

	dbg_assert(olist->__size == 0);
	dbg_assert(list_empty(&olist->__thelist));

	dbg_assert(false);

	return NULL;
}

static void* o_dlist_remove_back(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;

	//return r_dlist_remove_back(&olist->__cntr);
	if (olist->__size > 0) {
		struct list_link* link    = olist->__thelist.prev;
		struct o_dlist_node* node = container_of(link, struct o_dlist_node, link);
		void*  object_ref         = node->reference;

		dbg_assert(link != &olist->__thelist);

		list_unlink(link);
		//olist->__dealloc(node);
		dealloc(olist->__dealloc, olist->__heap, node);

		olist->__size --;

		return object_ref;
	}

	dbg_assert(olist->__size == 0);
	dbg_assert(list_empty(&olist->__thelist));

	dbg_assert(false);

	return NULL;
}

/* initialize all part of an iterator except the __current position */
static void o_dlist_itr_com_init(struct o_dlist_itr* itr, struct o_dlist* list) {
	itr->__offset = itr;
	itr->__cast   = o_dlist_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__dealloc = list->__dealloc;
	/* itr->__current = NULL; */
}

static object* o_dlist_itr_begin(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* n_itr = (struct o_dlist_itr*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_itr));

	o_dlist_itr_com_init(n_itr, olist);

	//n_itr->__current = &(r_dlist_first(&olist->__cntr)->link);
	/* if the list is empty, we just return the sentinel node */
	n_itr->__current = olist->__thelist.next;

	return (object*)n_itr;
}

static object* o_dlist_itr_end(object* o) {
	struct o_dlist* olist = (struct o_dlist*)o;
	struct o_dlist_itr* n_itr = (struct o_dlist_itr*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_itr));

	o_dlist_itr_com_init(n_itr, olist);

	/* the end iterator is the sentinel node, since 
	 * (begin, end) represents [begin, end) */
	n_itr->__current = &olist->__thelist;

	return (object*)n_itr;
}

static object* o_dlist_itr_find(object* o, void* ref) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* n_itr = (struct o_dlist_itr*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_itr));
	struct list_link* link    = olist->__thelist.next;
	struct o_dlist_node* node = NULL;

	while (link != &olist->__thelist) {
		struct o_dlist_node* temp = (struct o_dlist_node*)container_of(link, struct o_dlist_node, link);

		if (temp->reference == ref) {
			node = temp;
			break;
		}

		link = link->next;
	}

	o_dlist_itr_com_init(n_itr, olist);

	if (node != NULL) {
		n_itr->__current = &(node->link);
	}
	else {
		n_itr->__current = &olist->__thelist;
	}

	return (object*)n_itr;
}

static void* o_dlist_remove(object* o, iobject* itr) {
	struct o_dlist* olist     = (struct o_dlist*)o;
	struct o_dlist_itr* oitr  = (struct o_dlist_itr*)__object_from_interface(itr);
	struct o_dlist_node* node = (struct o_dlist_node*)container_of(oitr->__current, struct o_dlist_node, link);
	void* obj_ref             = node->reference;

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	list_unlink(&node->link);
	//olist->__dealloc(node);
	dealloc(olist->__dealloc, olist->__heap, node);

	olist->__size --;

	return obj_ref;
}

static void o_dlist_insert_before(object* o, iobject* itr, void* n_ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)__object_from_interface(itr);
	struct o_dlist_node* node   = (struct o_dlist_node*)container_of(oitr->__current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	//r_dlist_insert_before(&olist->__cntr, node, n_ref);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	olist->__size ++;
}

static void o_dlist_insert_after(object* o, iobject* itr, void* n_ref) {
	struct o_dlist* olist       = (struct o_dlist*)o;
	struct o_dlist_itr* oitr    = (struct o_dlist_itr*)__object_from_interface(itr);
	struct o_dlist_node* node   = (struct o_dlist_node*)container_of(oitr->__current, struct o_dlist_node, link);
	struct o_dlist_node* n_node = (struct o_dlist_node*)
		alloc(olist->__alloc, olist->__heap, sizeof(struct o_dlist_node));

	dbg_assert(oitr->__cast == o_dlist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	//r_dlist_insert_after(&olist->__cntr, node, n_ref);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	olist->__size ++;
}
