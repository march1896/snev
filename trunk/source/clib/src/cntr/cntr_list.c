#include <citer_base.h>
#include <citer_base.local.h>

#include <cntr_factory.h>
#include <cntr_linear.local.h>

#include <util/list_link.h>

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

static struct cntr_base_interface cntr_list_vtable = {
	cntr_list_attribute,     /* __attrib */
	cntr_list_destroy,       /* destroy */
	cntr_list_clear,         /* clean */
	cntr_list_size,          /* size */
	cntr_list_add,           /* add */
	cntr_list_remove,
	cntr_list_find,
	cntr_list_citer_begin,   /* citer_begin */
	cntr_list_citer_end  ,   /* citer_end   */
};

static struct cntr_linear_interface cntr_linear_vtable = {
	cntr_list_front,         /* front */
	cntr_list_back ,         /* back  */
	cntr_list_add_front,     /* add_front */
	cntr_list_add_back ,     /* add_back  */
	cntr_list_remove_front,  /* remove_front */
	cntr_list_remove_back ,  /* remove_back  */
};

typedef struct __cntr_list {
	/* start with pf_cast, so I am an object */
	pf_cast                       __cast;
	
	struct cntr_base_interface*   __base_intf;
	struct cntr_linear_interface* __linear_intf;

	pf_cntr_inner_alloc           __alloc;
	pf_cntr_inner_dealloc         __dealloc;
	pf_cntr_inner_dealloc         __obj_dealloc;  /* pre remove call back */

	int                           size;
	unsigned int                  flags;

	struct list_link              sent; /* sentinel */
} cntr_list;

typedef struct __open_link {
	struct list_link link;

	void*            object;
} object_link;

static unknown cntr_list_cast(unknown x, unique_id intf_id) {
	cntr_list* c = (cntr_list*)x;
	switch (intf_id) {
	case OBJECT_ME:
		return x;
	case CNTR_BASE_INTERFACE:
		return c->__base_intf;
	case CNTR_LINEAR_INTERFACE:
		return c->__linear_intf;
	default:
		return NULL;
	}

	return NULL;
}

cntr cntr_create_as_list() {
	return cntr_create_as_list_v(NULL, NULL, NULL);
}

cntr cntr_create_as_list_v(pf_cntr_inner_dealloc obj_dealloc, pf_cntr_inner_alloc alloc, pf_cntr_inner_dealloc dealloc) {
	cntr_list* pcl = NULL;

	if (alloc) {
		dbg_assert(dealloc != NULL);

		pcl = (cntr_list*)alloc(sizeof(cntr_list));
	}
	else {
		pcl = (cntr_list*)halloc(sizeof(cntr_list));
	}

	pcl->__cast        = cntr_list_cast;
	pcl->__base_intf   = &cntr_list_vtable;
	pcl->__linear_intf = &cntr_linear_vtable;
	pcl->flags         = 0;
	pcl->size          = 0;
	list_init(&pcl->sent);

	pcl->__alloc       = alloc;
	pcl->__dealloc     = dealloc;
	pcl->__obj_dealloc = obj_dealloc;

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

	/* destroy myself */
	pcl->__dealloc(pcl);
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
		if (pcl->__obj_dealloc) 
			pcl->__obj_dealloc(obj_link->object);
		/* second delete the link itself */
		pcl->__dealloc(obj_link);
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
	object_link* obj_link = (object_link*)pcl->__alloc(sizeof(object_link));

	obj_link->object = object;
	list_insert_front(&pcl->sent, &obj_link->link);
	
	pcl->size ++;
}

static void  cntr_list_add_back (cntr cl, void* object) {
	cntr_list* pcl = (cntr_list*)cl;
	object_link* obj_link = (object_link*)pcl->__alloc(sizeof(object_link));

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
		pcl->__dealloc(obj_link);

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
		pcl->__dealloc(obj_link);

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

		if (pcl->__obj_dealloc != NULL) 
			pcl->__obj_dealloc(obj_link->object);
		pcl->__dealloc(obj_link);

		count ++;
	};

	/* delete the last element */
	obj_link = container_of(lb, object_link, link);
	if (pcl->__obj_dealloc != NULL) 
		pcl->__obj_dealloc(obj_link->object);
	pcl->__dealloc(obj_link);
	count ++;
	
	pcl->size -= count;
}
