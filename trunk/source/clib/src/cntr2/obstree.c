#include <iset.h>
#include <iitr.h>

#include <ifactory.h>
#include <oallocator.h>

#include <util/llrb_link.h>

/* this module defines a left-lean red black tree container, which implements iset interface. */

enum llrb_interfaces {
	e_set,
	e_l_count
};

struct o_llrb_node {
	struct llrb_link              link;

	void*                         reference;
};

/* binary search tree */
struct o_llrb {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	struct llrb_link*             __treeroot;
	/* just a sentinel to represent the end of the tree, the maximum element of the tree */
	struct llrb_link              __sentinel;

	int                           __size;
	pf_compare                    __ref_comp;

	/* methods to manage the inner memory use by the container */
	allocator                     __allocator;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_dispose                    __dispose;
};

static object*  o_llrb_create          ();
static object*  o_llrb_create_v        (allocator alc, pf_dispose dispose);
static void     o_llrb_destroy         (object* o);
static void     o_llrb_clear           (object* o);
static int      o_llrb_size            (object* o);
static void     o_llrb_insert          (object* o, void* __ref);
static object*  o_llrb_itr_find        (object* o, void* __ref);
static void*    o_llrb_remove          (object* o, iobject* itr);
static object*  o_llrb_itr_begin       (object* o);
static object*  o_llrb_itr_end         (object* o);

/* factory method, the only public function in this file */
object* create_dblinked_list() {
	return o_llrb_create();
}
object* create_dblinked_list_v(allocator alc, pf_dispose dispose) {
	return o_llrb_create_v(alc, dispose);
}

static struct iset_vtable __iset_vtable = {
	o_llrb_destroy,          /* __destroy */
	o_llrb_clear,            /* __clear */
	o_llrb_size,             /* __size */
	o_llrb_insert,           /* __insert */
	o_llrb_itr_find,         /* __itr_find */
	o_llrb_remove,           /* __remove */
	o_llrb_itr_begin,        /* __itr_begin */
	o_llrb_itr_end           /* __itr_end */
};

typedef struct o_llrb_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	struct base_interface         __iftable[1];

	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     __allocator;

	struct llrb_link*             __current;
};


static unknown o_llrb_itr_cast(unknown x, unique_id inf_id);
static unknown o_llrb_cast(unknown x, unique_id intf_id);

static void o_llrb_itr_destroy(object* citr);
static bool o_llrb_itr_equals(object* a, object* b);
static void* o_llrb_itr_get_ref(object* citr);
static void o_llrb_itr_set_ref(object* citr, void* n_ref);
static void o_llrb_itr_to_next(object* citr);
static void o_llrb_itr_to_prev(object* citr);

static void o_llrb_itr_destroy(object* citr) {
	struct o_llrb_itr* itr = (struct o_llrb_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == o_llrb_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->__allocator, itr);
}

static bool o_llrb_itr_equals(object* a, object* b) {
	struct o_llrb_itr* itr_a = (struct o_llrb_itr*)a;
	struct o_llrb_itr* itr_b = (struct o_llrb_itr*)b;

	dbg_assert(__is_object(a));
	dbg_assert(__is_object(b));
	dbg_assert(itr_a->__cast == o_llrb_itr_cast);
	dbg_assert(itr_b->__cast == o_llrb_itr_cast);

	return itr_a->__current == itr_b->__current;
}

static void* o_llrb_itr_get_ref(object* citr) {
	struct o_llrb_itr* itr   = (struct o_llrb_itr*)citr;
	struct o_llrb_node* node = NULL;

	dbg_assert(itr->__cast == o_llrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = (struct o_llrb_node*)container_of(itr->__current, struct o_llrb_node, link);
	return node->reference;
}

static void o_llrb_itr_set_ref(object* citr, void* n_ref) {
	struct o_llrb_itr* itr   = (struct o_llrb_itr*)citr;
	struct o_llrb_node* node = NULL;

	dbg_assert(itr->__cast == o_llrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = (struct o_llrb_node*)container_of(itr->__current, struct o_llrb_node, link);
	node->reference = n_ref;
}

static void o_llrb_itr_to_next(object* citr) {
	struct o_llrb_itr* itr = (struct o_llrb_itr*)citr;

	dbg_assert(itr->__cast == o_llrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = llrb_successor(itr->__current, false);
}

static void o_llrb_itr_to_prev(object* citr) {
	struct o_llrb_itr* itr = (struct o_llrb_itr*)citr;

	dbg_assert(itr->__cast == o_llrb_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = llrb_predesessor(itr->__current, false);
}

static struct itr_bidirectional_vtable __itr_vtable = {
	o_llrb_itr_destroy,      /* __destroy */
	o_llrb_itr_equals,       /* __equals  */
	o_llrb_itr_get_ref,      /* __get_ref */
	o_llrb_itr_set_ref,      /* __set_ref */
	o_llrb_itr_to_next,      /* __to_next */
	o_llrb_itr_to_prev       /* __to_prev */
};

static unknown o_llrb_cast(unknown x, unique_id intf_id) {
	struct o_llrb* o = (struct o_llrb*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case ISET_ID:
		return (unknown)&o->__iftable[e_set];
	default:
		return NULL;
	}

	return NULL;
}

static unknown o_llrb_itr_cast(unknown x, unique_id inf_id) {
	struct o_llrb_itr* itr = (struct o_llrb_itr*)x;

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

static object* o_llrb_create(pf_compare ref_compare) {
	return o_llrb_create_v(ref_compare, global_llrb_allocator, NULL);
}

static object* o_llrb_create_v(pf_compare ref_compare, allocator alc, pf_dispose dispose) {
	struct o_llrb* olist = NULL;

	dbg_assert(alc != NULL);

	olist = (struct o_llrb*)allocator_alloc(alc, sizeof(struct o_llrb));

	olist->__offset = olist;
	olist->__cast   = o_llrb_cast;
	
	olist->__iftable[e_list].__offset = (address)e_list;
	olist->__iftable[e_list].__vtable = &__ilist_vtable;
	olist->__iftable[e_queue].__offset = (address)e_queue;
	olist->__iftable[e_queue].__vtable = &__iqueue_vtable;
	olist->__iftable[e_stack].__offset = (address)e_stack;
	olist->__iftable[e_stack].__vtable = &__istack_vtable;
	olist->__iftable[e_set].__offset = (address)e_set;
	olist->__iftable[e_set].__vtable = &__iset_vtable;

	olist->__size      = 0;
	olist->__ref_comp  = ref_compare;
	olist->__treeroot  = NULL;
	olist->__treeroot  = llrb_insert(olist->__treeroot, olist->__sentinel, llrb_link_compare);

	olist->__allocator = alc;

	olist->__dispose   = dispose;

	return (object*)olist;
}

static void per_link_dispose(struct list_link* link, void* param) {
	struct o_llrb_node* node = container_of(link, struct o_llrb_node, link);
	struct o_llrb* olist     = (struct o_llrb*)param;

	/* first dispose the reference */
	if (olist->__dispose) {
		olist->__dispose(node->reference);
	}
	
	/* TODO: should we destroy the sentinel node */
	
	/* delete the node it self */
	allocator_dealloc(olist->__allocator, node);
}

static void o_llrb_destroy(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;

	o_llrb_clear(o);
	allocator_dealloc(olist->__allocator, olist);
}

static void o_llrb_clear(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;

	list_foreach_v(&olist->__thelist, per_link_dispose, (void*)olist);

	list_init(&olist->__thelist);
	olist->__size = 0;
}

static int o_llrb_size(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;

	return olist->__size;
}

static void o_llrb_add_front(object* o, void* ref) {
	struct o_llrb* olist = (struct o_llrb*)o;

	struct o_llrb_node* n_node = (struct o_llrb_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_node));

	n_node->reference = ref;

	list_insert_front(&olist->__thelist, &n_node->link);
	olist->__size ++;
}

static void o_llrb_add_back(object* o, void* ref) {
	struct o_llrb* olist = (struct o_llrb*)o;

	struct o_llrb_node* n_node = (struct o_llrb_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_node));

	n_node->reference = ref;

	list_insert_back(&olist->__thelist, &n_node->link);
	olist->__size ++;
}

static void* o_llrb_remove_front(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;

	if (olist->__size > 0) {
		struct list_link* link    = olist->__thelist.next;
		struct o_llrb_node* node = container_of(link, struct o_llrb_node, link);
		void*  object_ref         = node->reference;

		dbg_assert(link != &olist->__thelist);

		list_unlink(link);
		allocator_dealloc(olist->__allocator, node);

		olist->__size --;

		return object_ref;
	}

	dbg_assert(olist->__size == 0);
	dbg_assert(list_empty(&olist->__thelist));

	dbg_assert(false);

	return NULL;
}

static void* o_llrb_remove_back(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;

	if (olist->__size > 0) {
		struct list_link* link    = olist->__thelist.prev;
		struct o_llrb_node* node = container_of(link, struct o_llrb_node, link);
		void*  object_ref         = node->reference;

		dbg_assert(link != &olist->__thelist);

		list_unlink(link);
		allocator_dealloc(olist->__allocator, node);

		olist->__size --;

		return object_ref;
	}

	dbg_assert(olist->__size == 0);
	dbg_assert(list_empty(&olist->__thelist));

	dbg_assert(false);

	return NULL;
}

/* initialize all part of an iterator except the __current position */
static void o_llrb_itr_com_init(struct o_llrb_itr* itr, struct o_llrb* list) {
	itr->__offset = itr;
	itr->__cast   = o_llrb_itr_cast;

	itr->__iftable[0].__offset = (address)0;
	itr->__iftable[0].__vtable = (unknown)&__itr_vtable;

	itr->__allocator = list->__allocator;
	/* itr->__current = NULL; */
}

static object* o_llrb_itr_begin(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;
	struct o_llrb_itr* n_itr = (struct o_llrb_itr*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_itr));

	o_llrb_itr_com_init(n_itr, olist);

	/* if the list is empty, we just return the sentinel node */
	n_itr->__current = olist->__thelist.next;

	return (object*)n_itr;
}

static object* o_llrb_itr_end(object* o) {
	struct o_llrb* olist = (struct o_llrb*)o;
	struct o_llrb_itr* n_itr = (struct o_llrb_itr*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_itr));

	o_llrb_itr_com_init(n_itr, olist);

	/* the end iterator is the sentinel node, since 
	 * (begin, end) represents [begin, end) */
	n_itr->__current = &olist->__thelist;

	return (object*)n_itr;
}

static object* o_llrb_itr_find(object* o, void* ref) {
	struct o_llrb* olist     = (struct o_llrb*)o;
	struct o_llrb_itr* n_itr = (struct o_llrb_itr*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_itr));
	struct list_link* link    = olist->__thelist.next;
	struct o_llrb_node* node = NULL;

	while (link != &olist->__thelist) {
		struct o_llrb_node* temp = (struct o_llrb_node*)container_of(link, struct o_llrb_node, link);

		if (temp->reference == ref) {
			node = temp;
			break;
		}

		link = link->next;
	}

	o_llrb_itr_com_init(n_itr, olist);

	if (node != NULL) {
		n_itr->__current = &(node->link);
	}
	else {
		n_itr->__current = &olist->__thelist;
	}

	return (object*)n_itr;
}

static void* o_llrb_remove(object* o, iobject* itr) {
	struct o_llrb* olist     = (struct o_llrb*)o;
	struct o_llrb_itr* oitr  = (struct o_llrb_itr*)__object_from_interface(itr);
	struct o_llrb_node* node = (struct o_llrb_node*)container_of(oitr->__current, struct o_llrb_node, link);
	void* obj_ref             = node->reference;

	dbg_assert(oitr->__cast == o_llrb_itr_cast);
	dbg_assert(oitr->__current != NULL);

	list_unlink(&node->link);
	//olist->__dealloc(node);
	allocator_dealloc(olist->__allocator, node);

	olist->__size --;

	return obj_ref;
}

static void o_llrb_insert_before(object* o, iobject* itr, void* n_ref) {
	struct o_llrb* olist       = (struct o_llrb*)o;
	struct o_llrb_itr* oitr    = (struct o_llrb_itr*)__object_from_interface(itr);
	struct o_llrb_node* node   = (struct o_llrb_node*)container_of(oitr->__current, struct o_llrb_node, link);
	struct o_llrb_node* n_node = (struct o_llrb_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_node));

	dbg_assert(oitr->__cast == o_llrb_itr_cast);
	dbg_assert(oitr->__current != NULL);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	olist->__size ++;
}

static void o_llrb_insert_after(object* o, iobject* itr, void* n_ref) {
	struct o_llrb* olist       = (struct o_llrb*)o;
	struct o_llrb_itr* oitr    = (struct o_llrb_itr*)__object_from_interface(itr);
	struct o_llrb_node* node   = (struct o_llrb_node*)container_of(oitr->__current, struct o_llrb_node, link);
	struct o_llrb_node* n_node = (struct o_llrb_node*)
		allocator_alloc(olist->__allocator, sizeof(struct o_llrb_node));

	dbg_assert(oitr->__cast == o_llrb_itr_cast);
	dbg_assert(oitr->__current != NULL);

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	olist->__size ++;
}
