#include <citer_base.h>
#include <citer_base.local.h>

#include <cntr_factory.h>
#include <cntr_linear.local.h>

#include <util/list_link.h>
#include <memheap/heap_global.h>

typedef struct __open_link {
	struct list_link link;

	void* object;
} object_link;

typedef struct __cntr_list_vtable {
	/* from cntr_linear_vtable */
	pf_cntr_attribute           __attrib;

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

static cattr cntr_list_attribute  (cntr cl);
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

typedef struct __cntr_list {
	cntr_list_vtable*           __vt;

	int                         size;
	unsigned int                flags;

	struct list_link            sent; /* sentinel */

	pf_preremove_cb             prerm;  /* pre remove call back */
} cntr_list;

static void cntr_list_init(cntr_list* pcl) {
	pcl->__vt = &cntr_list_ops;
	pcl->size = 0;
	pcl->flags = 0;
	list_init(&pcl->sent);
	pcl->prerm = NULL;
}

cntr cntr_create_as_list() {
	cntr_list* pcl = (cntr_list*)halloc(sizeof(cntr_list));

	cntr_list_init(pcl);

	return (cntr)pcl;
}

cntr cntr_create_as_list_r(pf_preremove_cb pre_rm) {
	cntr_list* pcl = (cntr_list*)halloc(sizeof(cntr_list));

	cntr_list_init(pcl);

	pcl->prerm = pre_rm;

	return (cntr)pcl;
}

/******************************************************************************************************
 * local functions begin
******************************************************************************************************/

static cattr cntr_list_attribute(cntr cl) {
	return CNTR_ATTR_BASE | CNTR_ATTR_LINEAR | CNTR_ATTR_LIST;
}

static void cntr_list_destroy(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	cntr_list_clear(cl);

	hfree(pcl);
}

static void cntr_list_clear(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	struct list_link* sent = &pcl->sent;
	struct list_link* next = sent->next;
	
	object_link* obj_link = NULL;

	dbg_assert(sent != NULL);

	while (next != sent) {
		obj_link = container_of(next, object_link, link);

		/* first delete the object memory */
		next = next->next;
		if (pcl->prerm) 
			pcl->prerm(obj_link->object);
		/* second delete the link itself */
		hfree(obj_link);
	}
	
	/* init the sentinel again, since we do not unlink the deleted nodes */
	list_init(sent);

	pcl->size = 0;
}

static int cntr_list_size(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;
	
	return pcl->size;
}

static void cntr_list_add(cntr cl, void* object) {
	cntr_list_add_back(cl, object);
}

static void* cntr_list_front(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;
	object_link* obj_link = NULL;

	if (pcl->size == 0) {
		dbg_assert(list_empty(&pcl->sent));
		return NULL;
	}
	
	obj_link = container_of(pcl->sent.next, object_link, link);
	return obj_link->object;
}

static void* cntr_list_back (cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;
	object_link* obj_link = NULL;

	if (pcl->size == 0) {
		dbg_assert(list_empty(&pcl->sent));
		return NULL;
	}

	obj_link = container_of(pcl->sent.prev, object_link, link);
	return obj_link->object;
}

static void  cntr_list_add_front(cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	object_link* obj_link = (object_link*)halloc(sizeof(object_link));

	obj_link->object = object;
	list_insert_front(&pcl->sent, &obj_link->link);
	
	pcl->size ++;
}

static void  cntr_list_add_back (cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	object_link* obj_link = (object_link*)halloc(sizeof(object_link));

	obj_link->object = object;
	list_insert_back(&pcl->sent, &obj_link->link);

	pcl->size ++;
}

static void* cntr_list_remove_front(cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->size > 0) {
		struct list_link* node = NULL;
		object_link* obj_link  = NULL;
		void* object           = NULL;

		dbg_assert(!list_empty(&pcl->sent));

		node = pcl->sent.next;
		obj_link = container_of(node, object_link, link);

		list_unlink(node);
		object = obj_link->object;
		hfree(obj_link);

		pcl->size --;

		return object;
	}

	dbg_assert(list_empty(&pcl->sent));

	return NULL;
}

static void*  cntr_list_remove_back (cntr cl) {
	cntr_list* pcl = (cntr_list*)cl;

	if (pcl->size > 0) {
		struct list_link* node = NULL;
		object_link* obj_link  = NULL;
		void* object           = NULL;

		dbg_assert(!list_empty(&pcl->sent));

		node = pcl->sent.prev;
		obj_link = container_of(node, object_link, link);

		list_unlink(node);
		object = obj_link->object;
		hfree(obj_link);

		pcl->size --;

		return object;
	}

	dbg_assert(list_empty(&pcl->sent));

	return NULL;
}

/* iterator related operations */

/* TODO: should we care about the validity of the iterator, or just leave it to the client, i++, i-- */
static void oplink_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	struct list_link* link_cur = (struct list_link*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->prev;
}

static void oplink_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	struct list_link* link_cur = (struct list_link*)(cur->connection);

	dbg_assert(link_cur);
	cur->connection = link_cur->next;
}

static void* oplink_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	struct list_link* link_cur = (struct list_link*)(cur->connection);
	object_link* obj_link = container_of(link_cur, object_link, link);

	dbg_assert(obj_link);
	return obj_link->object;
}

static void oplink_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	struct list_link* link_cur = (struct list_link*)(cur->connection);
	object_link* obj_link = container_of(link_cur, object_link, link);

	dbg_assert(obj_link);
	obj_link->object = n_ref;
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
	if (list_empty(&pcl->sent)) {
		itr->connection = NULL;
	}
	else {
		itr->connection = (void*)((&pcl->sent)->next);
	}
}

static void  cntr_list_citer_end  (cntr cl, citer cur) {
	citer_base* itr = (citer_base*)cur;
	cntr_list* pcl = (cntr_list*)cl;

	itr->__vt = &oplink_citer_operations;
	if (list_empty(&pcl->sent)) {
		itr->connection = NULL;
	}
	else {
		itr->connection = (void*)((&pcl->sent)->prev);
	}
}

static bool  cntr_list_find(cntr cl, void* object, citer itr) {
	cntr_list* pcl = (cntr_list*)cl;
	citer_base* ci = (citer_base*)itr;

	struct list_link* node = (&pcl->sent)->next;
	object_link* obj_link  = NULL;

	while (node != &pcl->sent) {
		obj_link = container_of(node, object_link, link);
		if (obj_link->object == object) {
			ci->connection = (void*)node;
			ci->__vt = &oplink_citer_operations;
			return true;
		}
		node = node->next;
	}

	ci->__vt = NULL;
	ci->connection = NULL;
	return false;
}

static void  cntr_list_remove(cntr cl, citer begin, citer end) {
	cntr_list* pcl = (cntr_list*)cl;

	struct list_link* lb = (struct list_link*)(((citer_base*)begin)->connection);
	struct list_link* le = (struct list_link*)(((citer_base*)end)->connection);
	object_link* obj_link = NULL;

	int count = 0;

	dbg_assert(lb != &pcl->sent && le != &pcl->sent);

	lb->prev->next = le->next;
	le->next->prev = lb->prev;

	while (lb != le) {
		obj_link = container_of(lb, object_link, link);
		lb = lb->next;

		if (pcl->prerm != NULL) 
			pcl->prerm(obj_link->object);
		hfree(obj_link);

		count ++;
	};

	/* delete the last element */
	obj_link = container_of(lb, object_link, link);
	if (pcl->prerm != NULL) 
		pcl->prerm(obj_link->object);
	hfree(obj_link);
	count ++;
	
	pcl->size -= count;
}
