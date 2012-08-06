#include <cntr_iterator.h>
#include <cntr_iterator.local.h>

#include <cntr_linear.local.h>

typedef struct open_link_t {
	struct open_link_t* prev;
	struct open_link_t* next;

	void* object;
} oplink;

typedef struct cntr_list_vtable_t {
	/* from cntr_linear_vtable */
	pf_attribute                __attrib;

	pf_cntr_base_destroy        __destroy;
	pf_cntr_base_clear          __clear;
	pf_cntr_base_size           __size;
	pf_cntr_base_add            __add;
	pf_cntr_base_remove         __remove;
	pf_cntr_base_find           __find;
	pf_cntr_base_citer_begin    __citer_begin;
	pf_cntr_base_citer_end      __citer_end;

	/* clinear specific */
	pf_cntr_linear_front        __front;
	pf_cntr_linear_back         __back;
	pf_cntr_linear_add_front    __add_front;
	pf_cntr_linear_add_back     __add_back;
	pf_cntr_linear_remove_front __remove_front;
	pf_cntr_linear_remove_back  __remove_back;

} cntr_list_vtable;

static cntr_attr cntr_list_attribute(cntr cl);
static void  cntr_list_destroy    (cntr cl);
static void  cntr_list_clear      (cntr cl);
static int   cntr_list_size       (cntr cl);
static void  cntr_list_add        (cntr cl, void* object);
static void  cntr_list_remove     (cntr cl, citer begin, citer end);
static bool  cntr_list_find       (cntr cl, void* object, citer itr);
static void  cntr_list_citer_begin(cntr cl, citer itr);
static void  cntr_list_citer_end  (cntr cl, citer itr);

static void* cntr_list_front      (cntr cl);
static void* cntr_list_back       (cntr cl);
static void  cntr_list_add_front  (cntr cl, void* object);
static void  cntr_list_add_back   (cntr cl, void* object);
static void* cntr_list_remove_front(cntr cl);
static void* cntr_list_remove_back (cntr cl);

static cntr_list_vtable cntr_list_ops = {
	cntr_list_attribute, /* __attrib */
	cntr_list_destroy, /* destroy */
	cntr_list_clear, /* clean */
	cntr_list_size, /* size */
	cntr_list_add, /* add */
	cntr_list_remove,
	cntr_list_find,
	cntr_list_citer_begin, /* citer_begin */
	cntr_list_citer_end  , /* citer_end   */

	cntr_list_front, /* front */
	cntr_list_back , /* back  */
	cntr_list_add_front, /* add_front */
	cntr_list_add_back , /* add_back  */
	cntr_list_remove_front, /* remove_front */
	cntr_list_remove_back , /* remove_back  */
};

typedef struct cntr_list_t {
	cntr_list_vtable*               __vt;

	int                         size;
	unsigned int                flags;
	oplink*                     begin;
	oplink*                     end;
} cntr_list;

cntr cntr_create_as_list() {
	cntr_list* pcl = (cntr_list*)halloc(sizeof(cntr_list));

	pcl->__vt = &cntr_list_ops;

	pcl->size = 0;
	pcl->flags = 0;
	pcl->begin = pcl->end = NULL;

	return (cntr)pcl;
}

/******************************************************************************************************
 * local functions begin
******************************************************************************************************/

static cntr_attr cntr_list_attribute(cntr cl) {
	return CNTR_ATTR_BASE | CNTR_ATTR_LINEAR | CNTR_ATTR_LIST;
}

static void cntr_list_destroy(cntr pcl) {
	cntr_list_clear(pcl);

	hfree(pcl);
}

static void cntr_list_clear(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	oplink* link = pcl->begin;
	oplink* prev = NULL;

	while (link != NULL) {
		prev = link;
		link = link->next;

		hfree(prev);
	}
}

static int cntr_list_size(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;
	
	return pcl->size;
}

static void cntr_list_add(cntr cl, void* object) {
	cntr_list_add_front(cl, object);
}

static void* cntr_list_front(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->begin == NULL) {
		dbg_assert(pcl->size == 0);
		return NULL;
	}

	return (pcl->begin)->object;
}

static void* cntr_list_back (cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->end == NULL) {
		dbg_assert(pcl->size == 0);

		return NULL;
	}

	return (pcl->end)->object;
}

static void  cntr_list_add_front(cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	oplink* link = (oplink*)halloc(sizeof(oplink));

	link->object = object;
	link->next = pcl->begin;
	link->prev = NULL;

	if (pcl->begin == NULL) {
		/* contains no element */
		dbg_assert(pcl->begin == NULL && pcl->size == 0);

		pcl->begin = pcl->end = link;
	}
	else {
		pcl->begin->prev = link;

		pcl->begin = link;
	}
	
	pcl->size ++;
}

static void  cntr_list_add_back (cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	oplink* link = (oplink*)halloc(sizeof(oplink));

	link->object = object;
	link->next = NULL;
	link->prev = pcl->end;

	if (pcl->end == NULL) {
		/* contains no element */
		dbg_assert(pcl->begin == NULL && pcl->size == 0);

		pcl->begin = pcl->end = link;
	}
	else {
		pcl->end->next = link;
		pcl->end = link;
	}

	pcl->size ++;
}

static void* cntr_list_remove_front(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;
	oplink* link = pcl->begin;
	void* object = link->object;
	oplink* next = link->next;

	dbg_assert(pcl->size > 0);
	if (next) next->prev = NULL;
	else {
		/* has only one element */
		dbg_assert(pcl->size == 1 && pcl->begin == pcl->end);
		pcl->end = NULL;
	}

	pcl->begin = next;
	hfree(link);

	pcl->size --;

	return object;
}

static void*  cntr_list_remove_back (cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;
	oplink* link = pcl->end;
	void* object = link->object;

	oplink* prev = link->prev;

	dbg_assert(pcl->size > 0);
	if (prev) prev->next = NULL;
	else {
		/* only one element */
		dbg_assert(pcl->size == 1 && pcl->begin == pcl->end);
		pcl->begin = NULL;
	}

	pcl->end = prev;
	hfree(link);

	pcl->size --;

	return object;
}

/* iterator related operations */

static void oplink_to_prev(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->prev;
}

static void oplink_to_next(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->next;
}

static bool oplink_valid(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	if (link_cur) return true;
	else return false;
}

static void* oplink_get_ref(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	return link_cur->object;
}

static void oplink_set_ref(citer itr, void* n_ref) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	link_cur->object = n_ref;
}

/*
static int oplink_cntr_size(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	cntr* list = (cntr*)(cur->container);

	dbg_assert(list);
	return list->size;
}
*/
static citer_interface oplink_citer_operations = {
	oplink_valid,
	oplink_get_ref,
	oplink_set_ref,
	oplink_to_prev,
	oplink_to_next,
};

static void  cntr_list_citer_begin(cntr cl, citer cur) {
	cntr_iterator* itr = (cntr_iterator*)cur;
	cntr_list* pcl = (cntr_list*)cl;

	itr->__vt = &oplink_citer_operations;
	itr->connection = (void*)pcl->begin;
}

static void  cntr_list_citer_end  (cntr cl, citer cur) {
	cntr_iterator* itr = (cntr_iterator*)cur;
	cntr_list* pcl = (cntr_list*)cl;

	itr->__vt = &oplink_citer_operations;
	itr->connection = (void*)pcl->end;
}

static bool  cntr_list_find(cntr cl, void* object, citer itr) {
	cntr_list* pcl = (cntr_list*)cl;
	cntr_iterator* ci = (cntr_iterator*)itr;

	oplink* link = pcl->begin;

	while (link != NULL) {
		if (link->object == object) {
			ci->connection = (void*)link;
			ci->__vt = &oplink_citer_operations;
			return true;
		}
		link = link->next;
	}

	ci->__vt = NULL;
	ci->connection = NULL;
	return false;
}

static void  cntr_list_remove(cntr cl, citer begin, citer end) {
	cntr_list* pcl = (cntr_list*)cl;

	oplink* link_begin = (oplink*)(((cntr_iterator*)begin)->connection);
	oplink* link_end = (oplink*)(((cntr_iterator*)end)->connection);

	oplink* link_prev = link_begin->prev;
	oplink* link_next = link_end->next;

	int count = 0;

	if (link_prev == NULL) {
		dbg_assert(pcl->begin == link_begin);
		pcl->begin = link_next;
	}
	else {
		link_prev->next = link_next;
	}

	if (link_next == NULL) {
		dbg_assert(pcl->end == link_end);
		pcl->end = link_prev;
	}
	else {
		link_next->prev = link_prev;
	}

	count = 1;
	while (link_begin != link_end) {
		link_begin = link_begin->next;
		count ++;
	}
	pcl->size -= count;
}
