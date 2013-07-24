#include <ilist.h>
#include <iqueue.h>
#include <istack.h>
#include <iset.h>
#include <iitr.h>

#include <util/list_link.h>
#include <dlist_ref.h>

enum list_interfaces {
	e_list,
	e_queue,
	e_stack,
	e_set,
	e_l_count
};

typedef struct o_dlinklist {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	struct dlist_ref              __cntr;

	/* methods to manage the inner memory use by the container */
	pf_alloc                      __alloc;
	pf_dealloc                    __dealloc;

	/* methods to manage the object's lifetime which is stored in the container */
	pf_copy                       __copy;
	pf_dispose                    __dispose;
};

object*  o_dlinklist_create          ();
object*  o_dlinklist_create_v        (pf_alloc alloc, pf_dealloc dealloc, pf_copy copy, pf_dispose dispose);
void     o_dlinklist_destroy         (object* o);
void     o_dlinklist_clear           (object* o);
int      o_dlinklist_size            (object* o);
void     o_dlinklist_add_front       (object* o, void* ref);
void     o_dlinklist_add_back        (object* o, void* ref);
void*    o_dlinklist_remove_front    (object* o);
void*    o_dlinklist_remove_back     (object* o);
object*  o_dlinklist_itr_find        (object* o, void* __ref);
void*    o_dlinklist_remove          (object* o, iobject* itr);
void     o_dlinklist_insert_before   (object* o, iobject* itr, void* n_ref);
void     o_dlinklist_insert_after    (object* o, iobject* itr, void* n_ref);
object*  o_dlinklist_itr_begin       (object* o);
object*  o_dlinklist_itr_end         (object* o);

static struct ilist_vtable __ilist_vtable = {
	o_dlinklist_destroy,          /* __destroy */
	o_dlinklist_clear,            /* __clear */
	o_dlinklist_size,             /* __size */
	o_dlinklist_add_front,        /* __add_front */
	o_dlinklist_add_back,         /* __add_back */
	o_dlinklist_remove_front,     /* __remove_front */
	o_dlinklist_remove_back,      /* __remove_back */
	o_dlinklist_itr_begin,        /* __itr_begin */
	o_dlinklist_itr_end,          /* __itr_end */
	o_dlinklist_itr_find,         /* __itr_find */
	o_dlinklist_remove,           /* __remove */
	o_dlinklist_insert_before,    /* __insert_before */
	o_dlinklist_insert_after      /* __insert_after */
};

static struct iqueue_vtable __iqueue_vtable = {
	o_dlinklist_destroy,          /* __destroy */
	o_dlinklist_clear,            /* __clear */
	o_dlinklist_size,             /* __size */
	o_dlinklist_add_back,         /* __push */
	o_dlinklist_remove_front,     /* __pop */
	o_dlinklist_itr_begin,        /* __itr_begin */
	o_dlinklist_itr_end           /* __itr_end */
};

static struct istack_vtable __istack_vtable = {
	o_dlinklist_destroy,          /* __destroy */
	o_dlinklist_clear,            /* __clear */
	o_dlinklist_size,             /* __size */
	o_dlinklist_add_back,         /* __push */
	o_dlinklist_remove_back,      /* __pop */
	o_dlinklist_itr_begin,        /* __itr_begin */
	o_dlinklist_itr_end           /* __itr_end */
};

static struct iset_vtable __iset_vtable = {
	o_dlinklist_destroy,          /* __destroy */
	o_dlinklist_clear,            /* __clear */
	o_dlinklist_size,             /* __size */
	o_dlinklist_add_back,           /* __insert */
	o_dlinklist_itr_find,         /* __itr_find */
	o_dlinklist_remove,           /* __remove */
	o_dlinklist_itr_begin,        /* __itr_begin */
	o_dlinklist_itr_end           /* __itr_end */
};

enum iter_interfaces {
	e_forward,
	e_bidir,
	/* e_randacc, */
	e_i_count
};

typedef struct o_dlinklist_itr {
	address                       __offset;
	pf_cast                       __cast;

	struct base_interface         __iftable[e_i_count];

	/* the iterator will never alloc memory, when aquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	pf_dealloc                    __dealloc;

	struct list_link*             __current;
	struct list_link*             __sentinel;
};


static unknown o_dlinklist_itr_cast(unknown x, unique_id inf_id);
static unknown o_dlinklist_cast(unknown x, unique_id intf_id);

void o_dlinklist_itr_destroy(object* citr);
bool o_dlinklist_itr_equals(object* a, object* b);
void o_dlinklist_itr_to_prev(object* citr);
void o_dlinklist_itr_to_next(object* citr);
void* o_dlinklist_itr_get_ref(object* citr);
void o_dlinklist_itr_set_ref(object* citr, void* n_ref);

void o_dlinklist_itr_destroy(object* citr) {
	struct o_dlinklist_itr* itr = (struct o_dlinklist_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == o_dlinklist_itr_cast);

	/* destroy itself */
	itr->__dealloc(itr);
}

bool o_dlinklist_itr_equals(object* a, object* b) {
	struct o_dlinklist_itr* itr_a = (struct o_dlinklist_itr*)a;
	struct o_dlinklist_itr* itr_b = (struct o_dlinklist_itr*)b;

	dbg_assert(__is_object(a));
	dbg_assert(__is_object(b));
	dbg_assert(itr_a->__cast == o_dlinklist_itr_cast);
	dbg_assert(itr_b->__cast == o_dlinklist_itr_cast);

	return itr_a->__current == itr_b->__current;
}

void o_dlinklist_itr_to_prev(object* citr) {
	struct o_dlinklist_itr* itr = (struct o_dlinklist_itr*)citr;

	dbg_assert(itr->__cast == o_dlinklist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = itr->__current->prev;

	if (itr->__current == itr->__sentinel) {
		itr->__current = NULL;
	}
}

void o_dlinklist_itr_to_next(object* citr) {
	struct o_dlinklist_itr* itr = (struct o_dlinklist_itr*)citr;

	dbg_assert(itr->__cast == o_dlinklist_itr_cast);
	dbg_assert(itr->__current != NULL);

	itr->__current = itr->__current->next;

	if (itr->__current == itr->__sentinel) {
		itr->__current = NULL;
	}
}

void* o_dlinklist_itr_get_ref(object* citr) {
	struct o_dlinklist_itr* itr = (struct o_dlinklist_itr*)citr;
	struct dlist_ref_node* node = NULL;

	dbg_assert(itr->__cast == o_dlinklist_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = (struct dlist_ref_node*)container_of(itr->__current, struct dlist_ref_node, link);
	return node->object;
}

void o_dlinklist_itr_set_ref(object* citr, void* n_ref) {
	struct o_dlinklist_itr* itr = (struct o_dlinklist_itr*)citr;
	struct dlist_ref_node* node = NULL;

	dbg_assert(itr->__cast == o_dlinklist_itr_cast);
	dbg_assert(itr->__current != NULL);

	node = (struct dlist_ref_node*)container_of(itr->__current, struct dlist_ref_node, link);
	node->object = n_ref;
}

static struct ifitr_vtable __ifitr_vtable = {
	o_dlinklist_itr_destroy,      /* __destroy */
	o_dlinklist_itr_equals,       /* __equals  */
	o_dlinklist_itr_to_next,      /* __to_next */
	o_dlinklist_itr_get_ref,      /* __get_ref */
	o_dlinklist_itr_set_ref       /* __set_ref */
};

static struct ibitr_vtable __ibitr_vtable = {
	o_dlinklist_itr_destroy,      /* __destroy */
	o_dlinklist_itr_equals,       /* __equals  */
	o_dlinklist_itr_to_prev,      /* __to_prev */
	o_dlinklist_itr_to_next,      /* __to_next */
	o_dlinklist_itr_get_ref,      /* __get_ref */
	o_dlinklist_itr_set_ref       /* __set_ref */
};

static unknown o_dlinklist_cast(unknown x, unique_id intf_id) {
	struct o_dlinklist* o = (struct o_dlinklist*)x;

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

static unknown o_dlinklist_itr_cast(unknown x, unique_id inf_id) {
	struct o_dlinklist_itr* itr = (struct o_dlinklist_itr*)x;

	dbg_assert(__is_object(itr));

	switch (inf_id) {
	case IFITR_ID:
		return (unknown)&itr->__iftable[e_forward];
	case IBITR_ID:
		return (unknown)&itr->__iftable[e_bidir];
	default:
		return NULL;
	}

	return NULL;
}

object* o_dlinklist_create() {
	return o_dlinklist_create_v(NULL, NULL, NULL, NULL);
}

object* o_dlinklist_create_v(pf_alloc alloc, pf_dealloc dealloc, pf_copy copy, pf_dispose dispose) {
	struct o_dlinklist* olist = NULL;
	pf_alloc __alloc = alloc != NULL ? alloc : sys_alloc;
	pf_dealloc __dealloc = dealloc != NULL ? dealloc : sys_dealloc;

	dbg_assert((alloc && dealloc) || (!alloc) && (!dealloc));

	olist = (struct o_dlinklist*)__alloc(sizeof(struct o_dlinklist));

	olist->__offset = olist;
	olist->__cast   = o_dlinklist_cast;
	
	olist->__iftable[e_list].__offset = (address)e_list;
	olist->__iftable[e_list].__vtable = &__ilist_vtable;
	olist->__iftable[e_queue].__offset = (address)e_queue;
	olist->__iftable[e_queue].__vtable = &__iqueue_vtable;
	olist->__iftable[e_stack].__offset = (address)e_stack;
	olist->__iftable[e_stack].__vtable = &__istack_vtable;
	olist->__iftable[e_set].__offset = (address)e_set;
	olist->__iftable[e_set].__vtable = &__iset_vtable;

	dlist_ref_init(&olist->__cntr, __alloc, __dealloc);

	olist->__alloc = alloc;
	olist->__dealloc = dealloc;

	olist->__copy  = copy;
	olist->__dispose = dispose;

	return (object*)olist;
}

void o_dlinklist_destroy(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	dlist_ref_clear(&olist->__cntr, olist->__dispose);
	olist->__dealloc(olist);
}

void o_dlinklist_clear(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	dlist_ref_clear(&olist->__cntr, olist->__dispose);
}

int o_dlinklist_size(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	return dlist_ref_size(&olist->__cntr);
}

void o_dlinklist_add_front(object* o, void* ref) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	dlist_ref_add_front(&olist->__cntr, ref);
}

void o_dlinklist_add_back(object* o, void* ref) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	dlist_ref_add_back(&olist->__cntr, ref);
}

void* o_dlinklist_remove_front(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	return dlist_ref_remove_front(&olist->__cntr);
}

void* o_dlinklist_remove_back(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;

	return dlist_ref_remove_back(&olist->__cntr);
}

/* initialize all part of an iterator except the __current position */
static void o_dlinklist_itr_com_init(struct o_dlinklist_itr* itr, struct o_dlinklist* list) {
	itr->__offset = itr;
	itr->__cast   = o_dlinklist_itr_cast;

	itr->__iftable[e_forward].__offset = (address)e_forward;
	itr->__iftable[e_forward].__vtable = (unknown)&__ifitr_vtable;
	itr->__iftable[e_bidir].__offset   = (address)e_bidir;
	itr->__iftable[e_bidir].__vtable   = (unknown)&__ifitr_vtable;

	itr->__dealloc = list->__dealloc;
	/* itr->__current = NULL; */

	itr->__sentinel = &list->__cntr.sent;
}

object* o_dlinklist_itr_begin(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;
	struct o_dlinklist_itr* n_itr = 
		(struct o_dlinklist_itr*)olist->__alloc(sizeof(struct o_dlinklist_itr));

	o_dlinklist_itr_com_init(n_itr, olist);

	n_itr->__current = &(dlist_ref_first(&olist->__cntr)->link);

	return (object*)n_itr;
}

object* o_dlinklist_itr_end(object* o) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;
	struct o_dlinklist_itr* n_itr = 
		(struct o_dlinklist_itr*)olist->__alloc(sizeof(struct o_dlinklist_itr));

	o_dlinklist_itr_com_init(n_itr, olist);

	n_itr->__current = &(dlist_ref_last(&olist->__cntr)->link);

	return (object*)n_itr;
}

object* o_dlinklist_itr_find(object* o, void* ref) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;
	struct o_dlinklist_itr* n_itr = 
		(struct o_dlinklist_itr*)olist->__alloc(sizeof(struct o_dlinklist_itr));

	o_dlinklist_itr_com_init(n_itr, olist);

	n_itr->__current = &(dlist_ref_find(&olist->__cntr, ref)->link);

	return (object*)n_itr;
}

void* o_dlinklist_remove(object* o, iobject* itr) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;
	struct o_dlinklist_itr* oitr = (struct o_dlinklist_itr*)__object_from_interface(itr);
	struct dlist_ref_node* node = NULL;
	void* obj_ref = NULL;
	dbg_assert(oitr->__cast == o_dlinklist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	node = (struct dlist_ref_node*)container_of(oitr->__current, struct dlist_ref_node, link);

	return dlist_ref_remove(&olist->__cntr, node);
}

void o_dlinklist_insert_before(object* o, iobject* itr, void* n_ref) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;
	struct o_dlinklist_itr* oitr = (struct o_dlinklist_itr*)__object_from_interface(itr);
	struct dlist_ref_node* node = NULL;

	dbg_assert(oitr->__cast == o_dlinklist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	node = (struct dlist_ref_node*)container_of(oitr->__current, struct dlist_ref_node, link);

	dlist_ref_insert_before(&olist->__cntr, node, n_ref);
}

void o_dlinklist_insert_after(object* o, iobject* itr, void* n_ref) {
	struct o_dlinklist* olist = (struct o_dlinklist*)o;
	struct o_dlinklist_itr* oitr = (struct o_dlinklist_itr*)__object_from_interface(itr);
	struct dlist_ref_node* node = NULL;

	dbg_assert(oitr->__cast == o_dlinklist_itr_cast);
	dbg_assert(oitr->__current != NULL);

	node = (struct dlist_ref_node*)container_of(oitr->__current, struct dlist_ref_node, link);

	dlist_ref_insert_after(&olist->__cntr, node, n_ref);
}
