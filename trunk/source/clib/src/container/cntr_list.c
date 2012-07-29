#include <cntr_list.h>
#include <cntr_iterator.h>

struct open_link_t {
	struct open_link_t* prev;
	struct open_link_t* next;

	void* object;
} ;

void* clist_front(clist* pcl) {
	if (pcl->begin == NULL) {
		assert(pcl->size == 0);
		return NULL;
	}

	return (pcl->begin)->object;
}

void* clist_back (clist* pcl) {
	if (pcl->end == NULL) {
		assert(pcl->size == 0);

		return NULL;
	}

	return (pcl->end)->object;
}

void  clist_add_front(clist* pcl, void* object) {
	oplink* link = (oplink*)halloc(sizeof(oplink));

	link->object = object;
	link->next = pcl->begin;
	link->prev = NULL;

	pcl->begin->prev = link;

	pcl->begin = link;
	if (!pcl->end) {
		assert(pcl->size == 0);
		pcl->end = link;
	}

	pcl->size ++;
}

void  clist_add_back (clist* pcl, void* object) {
	oplink* link = (oplink*)halloc(sizeof(oplink));

	link->object = object;
	link->next = NULL;
	link->prev = pcl->end;

	pcl->end->next = link;

	pcl->end = link;
	if (pcl->begin) {
		assert(pcl->size == 0);
		pcl->begin = link;
	}

	pcl->size ++;
}

void  clist_add      (clist* pcl, void* object) {
	clist_add_front(pcl, object);
}

void* clist_remove_front(clist* pcl) {
	assert(pcl->size > 0);

	oplink* link = pcl->begin;
	void* object = link->object;

	oplink* next = link->next;
	if (next) next->prev = NULL;
	else {
		/* has only one element */
		assert(pcl->size == 1 && pcl->begin == pcl->end);
		pcl->end = NULL;
	}

	pcl->begin = next;
	hfree(link);

	pcl->size --;

	return object;
}

void*  clist_remove_back (clist* pcl) {
	assert(pcl->size > 0);

	oplink* link = pcl->end;
	void* object = link->object;

	oplink* prev = link->prev;

	if (prev) prev->next = NULL;
	else {
		/* only one element */
		assert(pcl->size == 1 && pcl->begin == pcl->end);
		pcl->begin = NULL;
	}

	pcl->end = prev;
	hfree(link);

	pcl->size --;

	return object;
}

/* iterator related operations */

static void oplink_to_prev(citer* cur) {
	oplink* link_cur = (oplink*)(cur->connection);

	assert(link_cur);
	cur->connection = link_cur->prev;
}

static void oplink_to_next(citer* cur) {
	oplink* link_cur = (oplink*)(cur->connection);

	assert(link_cur);
	cur->connection = link_cur->next;
}

static bool oplink_valid(citer* cur) {
	oplink* link_cur = (oplink*)(cur->connection);

	if (link_cur) return true;
	else return false;
}

static void* oplink_get_ref(citer* cur) {
	oplink* link_cur = (oplink*)(cur->connection);

	assert(link_cur);
	return link_cur->object;
}

static citer_operations oplink_citer_operations = {
	.valid   = oplink_valid,
	.get_ref = oplink_get_ref,
	.to_prev = oplink_to_prev,
	.to_next = oplink_to_next
};

void  clist_citer_begin(clist* pcl, citer* itr) {
	itr->ops = &oplink_citer_operations;

	itr->connection = (void*)pcl->begin;
}

void  clist_citer_end  (clist* pcl, citer* itr) {
	itr->ops = &oplink_citer_operations;

	itr->connection = (void*)pcl->end;
}
