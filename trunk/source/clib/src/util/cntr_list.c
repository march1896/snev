#include <citer_base.h>
#include <citer_base.local.h>

#include <cntr_linear.local.h>

typedef struct open_link_t {
	struct open_link_t* prev;
	struct open_link_t* next;

	void* object;
} oplink;

typedef struct cntr_list_vtable_t {
	/* from cntr_linear_vtable */
	pf_cntr_attribute                __attrib;

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

static cattr cntr_list_attribute(cntr cl);
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
	oplink*                     s_sent;  /* the start sentinel */
	oplink*                     e_sent;    /* the end sentinel */
} cntr_list;

cntr cntr_create_as_list() {
	cntr_list* pcl = (cntr_list*)halloc(sizeof(cntr_list));
	oplink* s_sent, *e_sent;

	s_sent = (oplink*)halloc(sizeof(oplink));
	s_sent->object = NULL;
	s_sent->prev = NULL;

	e_sent = (oplink*)halloc(sizeof(oplink));
	e_sent->object = NULL;
	e_sent->prev = s_sent;
	e_sent->next = NULL;
	s_sent->next = e_sent;

	pcl->__vt = &cntr_list_ops;
	pcl->size = 0;
	pcl->flags = 0;
	pcl->s_sent = s_sent;
	pcl->e_sent = e_sent;

	return (cntr)pcl;
}

/******************************************************************************************************
 * local functions begin
******************************************************************************************************/

static cattr cntr_list_attribute(cntr cl) {
	return CNTR_ATTR_BASE | CNTR_ATTR_LINEAR | CNTR_ATTR_LIST;
}

static void cntr_list_destroy(cntr pcl) {
	cntr_list_clear(pcl);

	hfree(pcl);
}

static void cntr_list_clear(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	oplink* link = pcl->s_sent->next;
	oplink* prev = NULL;

	dbg_assert(link != NULL);

	while (link != pcl->e_sent) {
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

	if (pcl->s_sent->next == pcl->e_sent) {
		dbg_assert(pcl->size == 0 && pcl->e_sent->prev == pcl->s_sent);
		return NULL;
	}

	return (pcl->s_sent->next)->object;
}

static void* cntr_list_back (cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->s_sent->next == pcl->e_sent) {
		dbg_assert(pcl->size == 0 && pcl->e_sent->prev == pcl->s_sent);
		return NULL;
	}

	return (pcl->e_sent->prev)->object;
}

static inline void __link_insert(oplink* prev, oplink* cur, oplink* next) {
	dbg_assert(prev->next == next && next->prev == prev);

	cur->prev = prev;
	cur->next = next;

	prev->next = cur;
	next->prev = cur;
}

static inline void __link_stitch(oplink* prev, oplink* next) {
}

static void  cntr_list_add_front(cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	oplink* link = (oplink*)halloc(sizeof(oplink));

	link->object = object;
	link->next = pcl->s_sent->next;
	link->prev = pcl->s_sent;

	__link_insert(pcl->s_sent, link, pcl->s_sent->next);
	
	pcl->size ++;
}

static void  cntr_list_add_back (cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	oplink* link = (oplink*)halloc(sizeof(oplink));

	link->object = object;
	link->next = pcl->e_sent;
	link->prev = pcl->e_sent->prev;

	__link_insert(pcl->e_sent->prev, link, pcl->e_sent);

	pcl->size ++;
}

static void* cntr_list_remove_front(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->s_sent->next != pcl->e_sent) {
		oplink* link = pcl->s_sent->next;
		void* obj = link->object;

		dbg_assert(pcl->size > 0);
		__link_stitch(link->prev, link->next);
		pcl->size --;

		hfree(link);

		return obj;
	}

	return NULL;
}

static void*  cntr_list_remove_back (cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->s_sent->next != pcl->e_sent) {
		oplink* link = pcl->e_sent->prev;
		void* obj = link->object;

		dbg_assert(pcl->size > 0);
		__link_stitch(link->prev, link->next);
		pcl->size --;

		hfree(link);
		return obj;
	}

	return NULL;
}

/* iterator related operations */

static void oplink_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->prev;
}

static void oplink_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->next;
}

static void* oplink_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	return link_cur->object;
}

static void oplink_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	oplink* link_cur = (oplink*)(cur->connection);

	dbg_assert(link_cur);
	link_cur->object = n_ref;
}

static cattr oplink_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_LINK;
}

static citer_base_vtable oplink_citer_operations = {
	oplink_attribute,

	oplink_get_ref,
	oplink_set_ref,
	oplink_to_prev,
	oplink_to_next,
};

static void  cntr_list_citer_begin(cntr cl, citer cur) {
	citer_base* itr = (citer_base*)cur;
	cntr_list* pcl = (cntr_list*)cl;

	itr->__vt = &oplink_citer_operations;
	if (pcl->s_sent->next == pcl->e_sent) {
		itr->connection = NULL;
	}
	else itr->connection = (void*)(pcl->s_sent->next);
}

static void  cntr_list_citer_end  (cntr cl, citer cur) {
	citer_base* itr = (citer_base*)cur;
	cntr_list* pcl = (cntr_list*)cl;

	itr->__vt = &oplink_citer_operations;
	if (pcl->s_sent->next == pcl->e_sent) {
		itr->connection = NULL;
	}
	else itr->connection = (void*)(pcl->e_sent->prev);
}

static bool  cntr_list_find(cntr cl, void* object, citer itr) {
	cntr_list* pcl = (cntr_list*)cl;
	citer_base* ci = (citer_base*)itr;

	oplink* link = pcl->s_sent->next;

	while (link != pcl->e_sent) {
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

	oplink* lb = (oplink*)(((citer_base*)begin)->connection);
	oplink* le = (oplink*)(((citer_base*)end)->connection);
	oplink* prev;

	int count = 0;

	dbg_assert(lb->prev != NULL && le->next != NULL);

	__link_stitch(lb->prev, le->next);

	count = 1;
	while (lb != le) {
		prev = lb;
		lb = lb->next;

		hfree(prev);

		count ++;
	}

	hfree(le);
	pcl->size -= count;
}
