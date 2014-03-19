#include <string.h>

#include "iobject.h"
#include "iarray.h"
#include "iqueue.h"
#include "istack.h"
#include "iitr.h"
#include "ifactory.h"

#include "oarray.h"
#include "oallocator.h"
#include <memheap/heap_global.h>

enum list_interfaces {
	e_object,
	e_array,
	e_queue,
	e_stack,
	e_l_count
};

struct oarray;
struct oarray_itr {
	address                       __offset;
	pf_cast                       __cast;

	/* there is always one interface to implement, since the interface is inherited */
	struct base_interface         __iftable[itr_interface_count];

	struct oarray*                container;
	/* the iterator will never alloc memory, when acquire an iterator, the container will 
	 * alloc the memory, but we should know how to delete this memory */
	allocator                     allocator;

	int                           current; /* index of the element in the array */
};

struct oarray {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	int                           size;
	unknown_traits                content_traits;

	void**                        buffer;
	int                           buffer_length;
	int                           idx_start;  /* index of start position in the buffer */
	int                           idx_end;
	int                           threshhold; /* minimum buffer length */

	/* methods to manage the inner memory use by the container */
	allocator                     allocator;
	bool                          allocator_join_ondispose;

	struct oarray_itr             itr_begin;
	struct oarray_itr             itr_end;
};

static struct iobject_vtable __array_iobject_vtable = {
	oarray_destroy,          /* __destroy */
	oarray_clone,            /* __clone */
	oarray_equals,           /* __equals */
	oarray_compare_to,       /* __compare_to */
	oarray_hashcode,         /* __hashcode */
};

static struct iarray_vtable __array_iarray_vtable = {
	oarray_clear,            /* __clear */
	oarray_foreach,          /* __foreach */
	oarray_size,             /* __size */
	oarray_empty,            /* __empty */
	oarray_front,            /* __front */
	oarray_back,             /* __back */
	oarray_at,               /* __at */
	oarray_add_front,        /* __add_front */
	oarray_add_back,         /* __add_back */
	oarray_remove_front,     /* __remove_front */
	oarray_remove_back,      /* __remove_back */
	oarray_contains,         /* __contains */
	oarray_remove,           /* __remove  */

	oarray_itr_begin,        /* __itr_begin */
	oarray_itr_end,          /* __itr_end */
	oarray_itr_create,       /* __itr_create */
	oarray_itr_assign,       /* __itr_assign */
	oarray_itr_find,         /* __itr_find */
	oarray_itr_remove,       /* __itr_remove */
};

static struct iqueue_vtable __array_iqueue_vtable = {
	oarray_clear,            /* __clear */
	oarray_foreach,          /* __foreach */
	oarray_size,             /* __size */
	oarray_empty,            /* __empty */
	oarray_front,            /* __front */
	oarray_back,             /* __back */
	oarray_add_back,         /* __push */
	oarray_remove_front,     /* __pop */
	oarray_itr_create,       /* __itr_create */
	oarray_itr_assign,       /* __itr_assign */
	oarray_itr_begin,        /* __itr_begin */
	oarray_itr_end           /* __itr_end */
};

static struct istack_vtable __array_istack_vtable = {
	oarray_clear,            /* __clear */
	oarray_foreach,          /* __foreach */
	oarray_size,             /* __size */
	oarray_empty,            /* __empty */
	oarray_back,             /* __top */
	oarray_add_back,         /* __push */
	oarray_remove_back,      /* __pop */
	oarray_itr_create,       /* __itr_create */
	oarray_itr_assign,       /* __itr_assign */
	oarray_itr_begin,        /* __itr_begin */
	oarray_itr_end           /* __itr_end */
};

static unknown* oarray_itr_cast(unknown* x, unique_id inf_id);
static unknown* oarray_cast(unknown* x, unique_id intf_id);

static void oarray_itr_destroy(iterator citr);
static iterator oarray_itr_clone(const_iterator citr);
static bool oarray_itr_equals(const_iterator a, const_iterator b);
static int oarray_itr_compare_to(const_iterator itr, const_iterator other);
static hashcode oarray_itr_hashcode(const_iterator itr);
static unknown* oarray_itr_get_obj(const_iterator citr);
static void oarray_itr_set_obj(iterator citr, const unknown* n_ref);
static const unknown* oarray_itr_get_ref(const_iterator citr);
static void oarray_itr_swap_ref(iterator citr, iterator other);
static void oarray_itr_to_next(iterator citr);
static void oarray_itr_to_prev(iterator citr);
static void oarray_itr_advance(iterator citr, int length);
static int  oarray_itr_distance(const_iterator itr_a, const_iterator itr_b);

static void oarray_itr_destroy(iterator citr) {
	struct oarray_itr* itr = (struct oarray_itr*)citr;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oarray_itr_cast);

	/* destroy itself */
	allocator_dealloc(itr->allocator, itr);
}

static iterator oarray_itr_clone(const_iterator citr) {
	struct oarray_itr* itr = (struct oarray_itr*)citr;
	struct oarray_itr* n_itr = NULL;

	dbg_assert(__is_object(citr));
	dbg_assert(itr->__cast == oarray_itr_cast);

	n_itr = (struct oarray_itr*)allocator_alloc(itr->allocator, sizeof(struct oarray_itr));

	memcpy(n_itr, itr, sizeof(struct oarray_itr));
	/* this is error prone */
	n_itr->__offset = n_itr;

	return (iterator)n_itr;
}

static bool oarray_itr_equals(const_iterator a, const_iterator b) {
	const struct oarray_itr* itr_a = (const struct oarray_itr*)a;
	const struct oarray_itr* itr_b = (const struct oarray_itr*)b;

	dbg_assert(__is_object((unknown*)a));
	dbg_assert(__is_object((unknown*)b));
	dbg_assert(itr_a->__cast == oarray_itr_cast);
	dbg_assert(itr_b->__cast == oarray_itr_cast);

	return itr_a->current == itr_b->current;
}

static int oarray_itr_compare_to(const_iterator itr, const_iterator other) {
	// TODO
	dbg_assert(false);
	return 0;
}
static hashcode oarray_itr_hashcode(const_iterator itr) {
	// TODO
	dbg_assert(false);
	return 0;
}


#define INDEX(buf_len, start, index) (((start) + (index)) % (buf_len))
static inline unknown** BUFFER_AT(struct oarray* a, int index) {
	dbg_assert(index >= 0 && index < a->size);
	return &a->buffer[INDEX(a->buffer_length, a->idx_start, index)];
}

static unknown* oarray_itr_get_obj(const_iterator citr) {
	const struct oarray_itr* itr   = (const struct oarray_itr*)citr;
	struct oarray* container = itr->container;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->current != -1);

	/* return a internal reference as the iitr.h header said */
	return container->content_traits.__clone(*BUFFER_AT(container, itr->current), (pf_alloc)__global_default_alloc, __global_default_heap);
}

static void oarray_itr_set_obj(iterator citr, const unknown* n_ref) {
	struct oarray_itr* itr   = (struct oarray_itr*)citr;
	struct oarray* container = itr->container;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->current != -1);

	/* first destroy the old reference */
	container->content_traits.__destroy(*BUFFER_AT(container, itr->current), (pf_dealloc)allocator_release, container->allocator);

	/* then clone the new reference */
	*BUFFER_AT(container, itr->current) = 
		container->content_traits.__clone(n_ref, (pf_alloc)allocator_acquire, container->allocator);
}

static const unknown* oarray_itr_get_ref(const_iterator citr) {
	const struct oarray_itr* itr   = (const struct oarray_itr*)citr;
	struct oarray* container = itr->container;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->current != -1);

	/* return a internal reference as the iitr.h header said */
	return *BUFFER_AT(container, itr->current);
}

static void oarray_itr_swap_ref(iterator citr, iterator other) {
	struct oarray_itr* itr_a = (struct oarray_itr*)citr;
	struct oarray_itr* itr_b = (struct oarray_itr*)other;

	dbg_assert(itr_a->__cast == oarray_itr_cast);
	dbg_assert(itr_b->__cast == oarray_itr_cast);
	dbg_assert(itr_a->current != -1 && itr_b->current != -1);
	dbg_assert(itr_a->container == itr_b->container);

	if (itr_a->current == itr_b->current)
		return;

	{
		struct oarray* cntr = itr_a->container;
		/* TODO: so long, there must be something wrong */
		unknown* temp = *BUFFER_AT(cntr, itr_a->current);
		*BUFFER_AT(cntr, itr_a->current) = 
			*BUFFER_AT(cntr, itr_b->current);
		*BUFFER_AT(cntr, itr_b->current) = temp;
	}
}

static void oarray_itr_to_next(iterator citr) {
	struct oarray_itr* itr = (struct oarray_itr*)citr;
	struct oarray* cont    = itr->container;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->current != -1);
	dbg_assert(itr->current >= 0 && itr->current < cont->buffer_length);

	itr->current ++;
	if (itr->current == cont->buffer_length)
		itr->current = 0;
}

static void oarray_itr_to_prev(iterator citr) {
	struct oarray_itr* itr = (struct oarray_itr*)citr;
	struct oarray* cont    = itr->container;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->current != -1);
	dbg_assert(itr->current >= 0 && itr->current < cont->buffer_length);

	if (itr->current == 0)
		itr->current = cont->buffer_length;
	itr->current --;
}
/* like c array, there is no bound check for pointer arithmetic */
static void oarray_itr_advance(iterator citr, int length) {
	struct oarray_itr* itr = (struct oarray_itr*)citr;
	struct oarray* cont    = itr->container;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->current != -1);
	dbg_assert(itr->current >= 0 && itr->current < cont->buffer_length);

	itr->current = (itr->current + length + cont->buffer_length) % cont->buffer_length;
}
static int oarray_itr_distance(const_iterator citr, const_iterator other) {
	struct oarray_itr* itr_a = (struct oarray_itr*)citr;
	struct oarray_itr* itr_b = (struct oarray_itr*)other;
	struct oarray* a = itr_a->container;

	dbg_assert(itr_a->__cast == oarray_itr_cast);
	dbg_assert(itr_b->__cast == oarray_itr_cast);
	dbg_assert(itr_a->current != -1 && itr_b->current != -1);
	dbg_assert(itr_a->container == itr_b->container);

	return (itr_b->current - itr_a->current + a->buffer_length) % a->buffer_length;
}

static struct iobject_vtable __array_itr_iobject_vtable = {
	oarray_itr_destroy,      /* __destroy */
	oarray_itr_clone,        /* __clone   */
	oarray_itr_equals,       /* __equals  */
	oarray_itr_compare_to,   /* __compare_to */
	oarray_itr_hashcode
};

static struct itr_randomaccessible_vtable __array_itr_vtable = {
	oarray_itr_get_obj,      /* __get_obj */
	oarray_itr_get_ref,      /* __get_ref */
	oarray_itr_set_obj,      /* __set_obj */
	oarray_itr_swap_ref,     /* __swap_ref */
	oarray_itr_to_next,      /* __to_next */
	oarray_itr_to_prev,      /* __to_prev */
	oarray_itr_advance,      /* __advance */
	oarray_itr_distance      /* __distance */
};

static unknown* oarray_cast(unknown* x, unique_id intf_id) {
	struct oarray* o = (struct oarray*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IOBJECT_ID:
		return (unknown*)&o->__iftable[e_object];
	case IARRAY_ID:
		return (unknown*)&o->__iftable[e_array];
	case IQUEUE_ID:
		return (unknown*)&o->__iftable[e_queue];
	case ISTACK_ID:
		return (unknown*)&o->__iftable[e_stack];
	default:
		return NULL;
	}

	return NULL;
}

static unknown* oarray_itr_cast(unknown* x, unique_id inf_id) {
	struct oarray_itr* itr = (struct oarray_itr*)x;

	dbg_assert(__is_object(itr));

	switch (inf_id) {
	case IOBJECT_ID:
		return (unknown*)&itr->__iftable[itr_interface_iobject];
	case ITR_REF_ID:
	case ITR_ACC_ID:
	case ITR_FWD_ID:
	case ITR_BID_ID:
	case ITR_RAC_ID:
		return (unknown*)&itr->__iftable[itr_interface_iterator];
	default:
		return NULL;
	}

	return NULL;
}

_object* cntr_create_oarray(unknown_traits* content_traits) {
	return oarray_create(content_traits, __global_default_allocator);
}
_object* cntr_create_oarray_a(unknown_traits* content_traits, allocator alc) {
	return oarray_create(content_traits, alc);
}

static void oarray_adjust_buffer(struct oarray *a);
static void oarray_itr_com_init(struct oarray_itr* itr, struct oarray* list);
_object* oarray_create(unknown_traits* traits, allocator alc) {
	struct oarray* a = NULL;
	bool managed_allocator = false;

	if (alc == NULL) {
		alc = allocator_mpool_spawn(__global_default_allocator, 10);
		managed_allocator = true;
	}

	a = (struct oarray*)allocator_alloc(alc, sizeof(struct oarray));

	a->__offset = a;
	a->__cast   = oarray_cast;
	
	a->__iftable[e_object].__offset = (address)e_object;
	a->__iftable[e_object].__vtable = &__array_iobject_vtable;
	a->__iftable[e_array].__offset  = (address)e_array;
	a->__iftable[e_array].__vtable  = &__array_iarray_vtable;
	a->__iftable[e_queue].__offset  = (address)e_queue;
	a->__iftable[e_queue].__vtable  = &__array_iqueue_vtable;
	a->__iftable[e_stack].__offset  = (address)e_stack;
	a->__iftable[e_stack].__vtable  = &__array_istack_vtable;

	a->size = 0;

	a->content_traits.__destroy    = traits->__destroy;
	a->content_traits.__clone      = traits->__clone;
	a->content_traits.__compare_to = traits->__compare_to;
	a->content_traits.__equals     = traits->__equals;
	a->content_traits.__hashcode   = traits->__hashcode;

	a->buffer = NULL;
	a->buffer_length = 0;
	a->idx_start = 0;
	a->threshhold = 4;

	a->allocator = alc;
	a->allocator_join_ondispose = managed_allocator;

	oarray_adjust_buffer(a);

	oarray_itr_com_init(&a->itr_begin, a);
	oarray_itr_com_init(&a->itr_end, a);

	return (_object*)a;
}

static void oarray_adjust_buffer(struct oarray *a) {
	int n_length = -1;
	void** n_buffer = NULL;
	int threshhold = a->threshhold;
	int i = 0;

	if (a->buffer == NULL) {
		/* initialization */
		dbg_assert(a->size == 0);
		a->buffer = (void**)allocator_alloc(a->allocator, sizeof(void*)*threshhold);
		a->buffer_length = threshhold;
		a->idx_start = 0;
		return;
	}

	if (a->size >= a->buffer_length) {
		/* expand the buffer to double size */
		dbg_assert(a->size == a->buffer_length);
		n_length = 2 * a->size;
	} else if (a->size < a->buffer_length / 4) {
		/* shrink the buffer to half of the size */
		n_length = a->buffer_length / 2;

		n_length = n_length < threshhold ? threshhold : n_length;
	} else {
		/* no need to change the buffer size */
		return;
	}
	dbg_assert(a->size == a->buffer_length || a->size <= a->buffer_length / 4);
	dbg_assert(a->size <= n_length/2);

	if (n_length == a->buffer_length) {
		/* in case we touch the minimum threshhold */
		return;
	}

	n_buffer = (void**)allocator_alloc(a->allocator, sizeof(void*) * n_length);
	for (i = 0; i < a->size; i ++) {
		n_buffer[i] = a->buffer[INDEX(a->buffer_length, a->idx_start, i)];
	}
	/*
	if (a->idx_start <= a->idx_end) {
		memcpy(n_buffer, a->buffer + a->idx_start, sizeof(void*) * a->size);
	} else {
		int tail_len = a->buffer_length - a->idx_end;
		int head_len = a->idx_start;
		memcpy(n_buffer, a->buffer + a->idx_start, sizeof(void*) * tail_len);
		memcpy(n_buffer + tail_len, a->buffer, sizeof(void*) * head_len);
	}
	*/

	allocator_dealloc(a->allocator, a->buffer);
	a->buffer = n_buffer;
	a->buffer_length = n_length;
	a->idx_start = 0;
}

void oarray_destroy(_object* o) {
	struct oarray* a = (struct oarray*)o;
	allocator alc = a->allocator;
	bool join_alc = a->allocator_join_ondispose;

	oarray_clear(o);

	allocator_dealloc(a->allocator, a->buffer);
	allocator_dealloc(a->allocator, a);

	if (join_alc) {
		allocator_join(alc);
	}
}
_object* oarray_clone(const _object* o) {
	return NULL;
}
bool oarray_equals(const _object* o, const _object* other) {
	return false;
}
int oarray_compare_to(const _object* o, const _object* other) {
	return 0;
}
hashcode oarray_hashcode(const _object* o) {
	return 0;
}

void oarray_clear(_object* o) {
	struct oarray* a = (struct oarray*)o;
	int i = 0;

	for (i = 0; i < a->size; i ++) {
		a->content_traits.__destroy(*BUFFER_AT(a, i), (pf_dealloc)allocator_release, a->allocator);
	}

	a->size = 0;
	oarray_adjust_buffer(a);
}

void oarray_foreach(_object* o, pf_ref_process_v process, void* context) {
	struct oarray* a = (struct oarray*)o;
	int i = 0;

	for (i = 0; i < a->size; i ++) {
		process(*BUFFER_AT(a, i), context);;
	}
}

int oarray_size(const _object* o) {
	const struct oarray* a = (const struct oarray*)o;

	return a->size;
}

bool oarray_empty(const _object* o) {
	const struct oarray* a = (const struct oarray*)o;

	return a->size == 0;
}

const unknown* oarray_front(const _object* o) {
	const struct oarray* a = (const struct oarray*)o;

	if (a->size == 0) {
		return NULL;
	}

	return a->buffer[a->idx_start];
}

const unknown* oarray_back(const _object* o) {
	struct oarray* a = (struct oarray*)o;

	if (a->size == 0) {
		return NULL;
	}

	return *BUFFER_AT(a, a->size-1);
}

const unknown* oarray_at(const _object* o, int index) {
	struct oarray* a = (struct oarray*)o;

	index = ((index % a->size) + a->size) % a->size;

	return *BUFFER_AT(a, index);
}

void oarray_add_front(_object* o, const unknown* __ref) {
	struct oarray* a = (struct oarray*)o;

	dbg_assert(a->size < a->buffer_length);
	a->idx_start --;
	if (a->idx_start < 0) 
		a->idx_start += a->buffer_length;

	a->buffer[a->idx_start] = a->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, a->allocator);
	a->size ++;

	oarray_adjust_buffer(a);
}

void oarray_add_back(_object* o, const unknown* __ref) {
	struct oarray* a = (struct oarray*)o;
	int idx_end = INDEX(a->buffer_length, a->idx_start, a->size);

	dbg_assert(a->size < a->buffer_length);

	a->buffer[idx_end] = a->content_traits.__clone(__ref, (pf_alloc)allocator_acquire, a->allocator);

	a->size ++;
	oarray_adjust_buffer(a);
}

void oarray_remove_front(_object* o) {
	struct oarray* a = (struct oarray*)o;

	if (a->size <= 0)
		return;

	a->content_traits.__destroy(a->buffer[a->idx_start], (pf_dealloc)allocator_release, a->allocator);
	a->buffer[a->idx_start] = NULL;

	a->idx_start ++;
	if (a->idx_start >= a->buffer_length) 
		a->idx_start -= a->buffer_length;

	a->size --;

	oarray_adjust_buffer(a);
}

void oarray_remove_back(_object* o) {
	struct oarray* a = (struct oarray*)o;
	int idx_last = INDEX(a->buffer_length, a->idx_start, a->size-1);

	if (a->size <= 0)
		return;

	a->content_traits.__destroy(a->buffer[idx_last], (pf_dealloc)allocator_release, a->allocator);
	a->buffer[idx_last] = NULL;

	a->size --;

	oarray_adjust_buffer(a);
}

bool oarray_contains(const _object* o, const unknown* __ref) {
	struct oarray* a  = (struct oarray*)o;
	int i = 0;

	for (i = 0; i < a->size; i ++) {
		if (a->content_traits.__equals(*BUFFER_AT(a, i), __ref)) {
			return true;
		}
	}

	return false;
}

static void oarray_remove_at(struct oarray* a, int index) {
	int blen = a->buffer_length;
	int idx  = (a->idx_start + index) % blen;

	dbg_assert(index >= 0 && index < a->size);
	a->content_traits.__destroy(a->buffer[idx], (pf_dealloc)allocator_release, a->allocator);
	if (index * 2 < a->size) {
		/* shift the content from idx_start to index */
		while (index > 0) {
			a->buffer[(idx + blen)% blen] = a->buffer[(idx-1+blen) % blen];
			idx --;
			index --;
		}
		a->idx_start = (a->idx_start + 1) % a->buffer_length;
	}
	else {
		/* shift the content from index to end*/
		while (index + 1 < a->size) {
			a->buffer[(idx + blen) % blen] = a->buffer[(idx+1+blen) % blen];
			idx ++;
			index ++;
		}
	}
}

bool oarray_remove(_object* o, const unknown* __ref) {
	struct oarray* a  = (struct oarray*)o;
	int i = 0;

	for (i = 0; i < a->size; i ++) {
		if (a->content_traits.__equals(*BUFFER_AT(a, i), __ref)) {
			break;
		}
	}

	if (i == a->size)
		return false;

	oarray_remove_at(a, i);

	a->size --;
	oarray_adjust_buffer(a);
	return true;
}


/* initialize all part of an iterator except the __current position */
static void oarray_itr_com_init(struct oarray_itr* itr, struct oarray* a) {
	itr->__offset = itr;
	itr->__cast   = oarray_itr_cast;

	itr->__iftable[itr_interface_iobject].__offset = (address)itr_interface_iobject;
	itr->__iftable[itr_interface_iobject].__vtable = (void*)&__array_itr_iobject_vtable;
	itr->__iftable[itr_interface_iterator].__offset = (address)itr_interface_iterator;
	itr->__iftable[itr_interface_iterator].__vtable = (void*)&__array_itr_vtable;

	itr->container = a;
	itr->allocator = a->allocator;
	/* itr->__current = NULL; */
}

const_iterator oarray_itr_begin(const _object* o) {
	struct oarray* a = (struct oarray*)o;

	/* if the list is empty, we just return the sentinel node */
	a->itr_begin.current = 0;

	return (const_iterator)&a->itr_begin;
}

const_iterator oarray_itr_end(const _object* o) {
	struct oarray* a = (struct oarray*)o;

	a->itr_end.current = a->size;

	return (const_iterator)&a->itr_end;
}

iterator oarray_itr_create(const _object* o, itr_pos pos) {
	struct oarray* a = (struct oarray*)o;
	struct oarray_itr* n_itr = (struct oarray_itr*)
		allocator_alloc(a->allocator, sizeof(struct oarray_itr));

	oarray_itr_com_init(n_itr, a);

	if (pos == itr_begin) {
		/* if the list is empty, we just return the sentinel node */
		n_itr->current = 0;
	}
	else if (pos == itr_end) {
		/* the end iterator is the sentinel node, since 
		 * (begin, end) represents [begin, end) */
		n_itr->current = a->size;
	}
	else {
		dbg_assert(false);
	}

	return (iterator)n_itr;
}

void oarray_itr_assign(const _object* o, iterator __itr, itr_pos pos) {
	struct oarray* a = (struct oarray*)o;
	struct oarray_itr* itr = (struct oarray_itr*)__itr;

	dbg_assert(itr->__cast == oarray_itr_cast);
	dbg_assert(itr->container == a);
	dbg_assert(itr->current != -1);

	oarray_itr_com_init(itr, a);

	if (pos == itr_begin) {
		/* if the list is empty, we just return the sentinel node */
		itr->current = 0;
	}
	else if (pos == itr_end) {
		/* the end iterator is the sentinel node, since 
		 * (begin, end) represents [begin, end) */
		itr->current = a->size;
	}
	else {
		dbg_assert(false);
	}

	return;
}

void oarray_itr_find(const _object* o, iterator itr, const unknown* __ref) {
	struct oarray* a    = (struct oarray*)o;
	struct oarray_itr* oitr = (struct oarray_itr*)itr;
	int i = 0;

	/* make sure the type information is right */
	dbg_assert(itr->__iftable[itr_interface_iterator].__offset == (address)itr_interface_iterator);
	dbg_assert(itr->__iftable[itr_interface_iterator].__vtable == (void*)&__array_itr_vtable);

	dbg_assert(oitr->container == a);

	for (i = 0; i < a->size; i ++) {
		if (a->content_traits.__equals(*BUFFER_AT(a, i), __ref)) {
			break;
		}
	}

	oitr->current = i;
}

void oarray_itr_remove(_object* o, iterator itr) {
	struct oarray* a         = (struct oarray*)o;
	struct oarray_itr* oitr  = (struct oarray_itr*)itr;
	int index                = oitr->current;

	dbg_assert(oitr->__cast == oarray_itr_cast);
	dbg_assert(oitr->container == a);
	dbg_assert(oitr->current != -1);
	dbg_assert(index >= 0 && index < a->size);

	oarray_remove_at(a, index);

	a->size --;
	oarray_adjust_buffer(a);

	/* invalidate the iterator */
	oitr->current = -1;

	return;
}
