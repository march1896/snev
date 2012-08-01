#include <cntr_list.h>
#include <cntr_iterator.h>
#include <cntr_iterator.ph>

struct open_link_t {
	struct open_link_t* prev;
	struct open_link_t* next;

	void* object;
};

typedef struct open_link_t oplink;

typedef struct cntr_list_t {
	int     size;
	unsigned flags;

	oplink* begin;
	oplink* end;
} clist;

void clist_init(clist* pcl) {
	pcl->size = 0;
	pcl->flags = 0;
	pcl->begin = pcl->end = NULL;
}

void clist_deinit(clist* pcl) {
	/* DO NOTHING */
}

void clist_clean(clist* pcl) {
	oplink* link = pcl->begin;
	oplink* prev = NULL;

	while (link != NULL) {
		prev = link;
		link = link->next;

		hfree(prev);
	}
}

int clist_size(clist* pcl) {
	return pcl->size;
}

void* clist_front(clist* pcl) {
	if (pcl->begin == NULL) {
		dbg_assert(pcl->size == 0);
		return NULL;
	}

	return (pcl->begin)->object;
}

void* clist_back (clist* pcl) {
	if (pcl->end == NULL) {
		dbg_assert(pcl->size == 0);

		return NULL;
	}

	return (pcl->end)->object;
}

void  clist_add_front(clist* pcl, void* object) {
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

void  clist_add_back (clist* pcl, void* object) {
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

void  clist_add      (clist* pcl, void* object) {
	clist_add_front(pcl, object);
}

void* clist_remove_front(clist* pcl) {
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

void*  clist_remove_back (clist* pcl) {
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

static void oplink_to_prev(citer* itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->prev;
}

static void oplink_to_next(citer* itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->next;
}

static bool oplink_valid(citer* itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	if (link_cur) return true;
	else return false;
}

static void* oplink_get_ref(citer* itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	return link_cur->object;
}

static void oplink_set_ref(citer* itr, void* n_ref) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	link_cur->object = n_ref;
}

static int oplink_cntr_size(citer* itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	clist* list = (clist*)(cur->container);

	dbg_assert(list);
	return list->size;
}

static citer_operations oplink_citer_operations = {
#if defined(__GCC__)
	.valid   = oplink_valid,
	.get_ref = oplink_get_ref,
	.set_ref = oplink_set_ref,
	.to_prev = oplink_to_prev,
	.to_next = oplink_to_next,
	.cntr_size = oplink_cntr_size;
#else 
	oplink_valid,
	oplink_get_ref,
	oplink_set_ref,
	oplink_to_prev,
	oplink_to_next,
	oplink_cntr_size
#endif 
};

void  clist_citer_begin(clist* pcl, citer* cur) {
	cntr_iterator* itr = (cntr_iterator*)cur;
	itr->ops = &oplink_citer_operations;

	itr->connection = (void*)pcl->begin;
	itr->container = (void*)pcl;
}

void  clist_citer_end  (clist* pcl, citer* cur) {
	cntr_iterator* itr = (cntr_iterator*)cur;
	itr->ops = &oplink_citer_operations;

	itr->connection = (void*)pcl->end;
	itr->container = (void*)pcl;
}
