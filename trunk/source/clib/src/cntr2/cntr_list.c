#include <cntr_list.h>

#include <util/list_link.h>

/* TODO */
void* sys_alloc(int size);
void  sys_dealloc(void* buff);

inline void cntr_list_init(struct cntr_list* cl, pf_alloc __alloc, pf_dealloc __dealloc) {
	dbg_assert(__alloc   != NULL);
	dbg_assert(__dealloc != NULL);

	cl->alloc     = __alloc;
	cl->dealloc   = __dealloc;
	cl->size      = 0;

	list_init(&cl->sent);
}

inline void cntr_list_clear(struct cntr_list* cl, pf_obj_dispose __dispose) {
	struct list_link* sent = &cl->sent;
	struct list_link* next = sent->next;
	struct list_node* node = NULL;

	dbg_assert(sent != NULL);

	while (next != sent) {
		node = container_of(next, struct list_node, link);

		next = next->next;

		/* first delete the object memory */
		if (__dispose) 
			__dispose(node->object);
		/* second delete the link itself */
		cl->dealloc(node);
	}
	
	/* init the sentinel again, since we do not unlink the deleted nodes */
	list_init(sent);

	cl->size = 0;
}

inline int cntr_list_size(struct cntr_list* cl) {
	return cl->size;
}

inline void cntr_list_add(struct cntr_list* cl, void* object) {
	cntr_list_add_back(cl, object);
}

inline struct list_node* cntr_list_find(struct cntr_list* cl, void* object) {
	struct list_link* link = cl->sent.next;
	struct list_node* node = NULL;

	while (link != &cl->sent) {
		node = container_of(link, struct list_node, link);

		if (node->object == object) {
			return node;
		}

		link = link->next;
	}

	return NULL;
}

inline struct list_node* cntr_list_first(struct cntr_list* cl) {
	struct list_node* node = NULL;

	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		return NULL;
	}

	node = container_of(cl->sent.next, struct list_node, link);
	return node;
}
inline struct list_node* cntr_list_last(struct cntr_list* cl) {
	struct list_node* node = NULL;

	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		return NULL;
	}

	node = container_of(cl->sent.prev, struct list_node, link);
	return node;
}

inline void* cntr_list_front(struct cntr_list* cl) {
	struct list_node* node = cntr_list_first(cl);

	if (node) {
		return node->object;
	}

	dbg_assert(cl->size == 0);
	return NULL;
}

inline void* cntr_list_back(struct cntr_list* cl) {
	struct list_node* node = cntr_list_last(cl);

	if (node) {
		return node->object;
	}

	dbg_assert(cl->size == 0);
	return NULL;
}

inline void cntr_list_add_front(struct cntr_list* cl, void* object) {
	struct list_node* n_node = cl->alloc(sizeof(struct list_node));

	n_node->object = object;
	list_insert_front(&cl->sent, &n_node->link);

	cl->size ++;
}

inline void cntr_list_add_back(struct cntr_list* cl, void* object) {
	struct list_node* n_node = cl->alloc(sizeof(struct list_node));

	n_node->object = object;
	list_insert_back(&cl->sent, &n_node->link);

	cl->size ++;
}

inline void* cntr_list_remove_front(struct cntr_list* cl) {
	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		dbg_assert(false);
	}

	struct list_link* link = cl->sent.next;
	struct list_node* node = container_of(link, struct list_node, link);
	void*  object_ref      = node->object;

	list_unlink(link);
	cl->dealloc(node);

	cl->size --;

	return object_ref;
}

inline void* cntr_list_remove_back(struct cntr_list* cl) {
	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		dbg_assert(false);
	}

	struct list_link* link = cl->sent.prev;
	struct list_node* node = container_of(link, struct list_node, link);
	void*  object_ref      = node->object;

	list_unlink(link);
	cl->dealloc(node);

	cl->size --;

	return object_ref;
}
