//#include <ilist.h>
#include <iqueue.h>
#include <istack.h>
#include <iset.h>

#include <util/list_link.h>

#include <cntr_list.h>

typedef unknown cntr;
typedef unknown citer;

static object* cntr_list_create   ();
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

static struct inf_cntr_base_vtable cntr_base_vtable = {
	cntr_list_destroy,       /* destroy */
	cntr_list_clear,         /* clean */
	cntr_list_size,          /* size */
	cntr_list_add,           /* add */
	cntr_list_remove,
	cntr_list_find,
	cntr_list_citer_begin,   /* citer_begin */
	cntr_list_citer_end  ,   /* citer_end   */
};

static struct inf_cntr_linear_vtable cntr_linear_vtable = {
	cntr_list_front,         /* front */
	cntr_list_back ,         /* back  */
	cntr_list_add_front,     /* add_front */
	cntr_list_add_back ,     /* add_back  */
	cntr_list_remove_front,  /* remove_front */
	cntr_list_remove_back ,  /* remove_back  */
};

enum list_interfaces {
	e_list,
	e_queue,
	e_stack,
	e_set,
	e_l_count
};

enum iter_interfaces {
	e_forward,
	e_bidir,
	e_randacc,
	e_i_count
};

typedef struct class_list {
	address                       __address;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_l_count];

	struct cntr_list              __cntr;
} clist;

typedef struct class_list_itr {
	address                       __address;
	pf_cast                       __cast;

	struct base_interface         __iftable[e_i_count];

	void*                         __connection;
} clist_itr;

static unknown cntr_list_cast(unknown x, unique_id intf_id) {
	cntr_list* c = (cntr_list*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case ILIST_ID:
		return (unknown)&c->__iftable[e_list];
	case IQUEUE_ID:
		return (unknown)&c->__iftable[e_queue];
	case ISTACK_ID:
		return (unknown)&c->__iftable[e_stack];
	case ISET_ID:
		return (unknown)&c->__iftable[e_set];
	default:
		return NULL;
	}

	return NULL;
}

static unknown cntr_list_itr_cast(unknown x, unique_id inf_id) {
	cntr_list_itr* itr = (cntr_list_itr*)x;

	dbg_assert(__is_object(itr));

	switch (inf_id) {
	case IFITR_ID:
		return (unknown)&itr->__iftable[e_forward];
	case IBITR_ID:
		return (unknown)&itr->__iftable[e_bidir];
	case IRITR_ID:
		return (unknown)&itr->__iftable[e_randacc];
	default:
		return NULL;
	}

	return NULL;
}

cntr cntr_create_as_list_v(pf_dealloc obj_dispose, pf_alloc alloc, pf_dispose dealloc) {
	cntr_list* pcl = NULL;

	if (alloc) {
		dbg_assert(dealloc != NULL);

		pcl = (cntr_list*)alloc(sizeof(cntr_list));
	}
	else {
		pcl = (cntr_list*)halloc(sizeof(cntr_list));
	}

	pcl->__address     = __address_of(pcl);
	pcl->__cast        = cntr_list_cast;
	
	/* set the interfaces */
	pcl->__iftable[CNTR_BASE_INTERFACE_OFFSET].__offset   = (address)CNTR_BASE_INTERFACE_OFFSET;
	pcl->__iftable[CNTR_BASE_INTERFACE_OFFSET].__vtable   = &cntr_base_vtable;
	pcl->__iftable[CNTR_LINEAR_INTERFACE_OFFSET].__offset = (address)CNTR_LINEAR_INTERFACE_OFFSET;
	pcl->__iftable[CNTR_LINEAR_INTERFACE_OFFSET].__vtable = &cntr_linear_vtable;

	pcl->flags         = 0;
	pcl->size          = 0;
	list_init(&pcl->sent);

	pcl->__alloc       = alloc;
	pcl->__dealloc     = dealloc;
	pcl->__obj_dispose = obj_dispose;

	return (cntr)pcl;
}

cntr cntr_create_as_list() {
	return cntr_create_as_list_v(NULL, NULL, NULL);
}

/******************************************************************************************************
 * local functions begin
******************************************************************************************************/

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
		if (pcl->__obj_dispose) 
			pcl->__obj_dispose(obj_link->object);
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

		if (pcl->__obj_dispose != NULL) 
			pcl->__obj_dispose(obj_link->object);
		pcl->__dealloc(obj_link);

		count ++;
	};

	/* delete the last element */
	obj_link = container_of(lb, object_link, link);
	if (pcl->__obj_dispose != NULL) 
		pcl->__obj_dispose(obj_link->object);
	pcl->__dealloc(obj_link);
	count ++;
	
	pcl->size -= count;
}
