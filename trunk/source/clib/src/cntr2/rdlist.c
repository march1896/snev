#include <rdlist.h>

#include <util/list_link.h>

inline void r_dlist_init(struct r_dlist* cl, pf_alloc __alloc, pf_dealloc __dealloc) {
	dbg_assert(__alloc   != NULL);
	dbg_assert(__dealloc != NULL);

	cl->__alloc     = __alloc;
	cl->__dealloc   = __dealloc;
	cl->size      = 0;

	list_init(&cl->sent);
}

inline void r_dlist_clear(struct r_dlist* cl, pf_dispose __dispose) {
	struct list_link* sent = &cl->sent;
	struct list_link* next = sent->next;
	struct r_dlist_node* node = NULL;

	dbg_assert(sent != NULL);

	while (next != sent) {
		node = container_of(next, struct r_dlist_node, link);

		next = next->next;

		/* first delete the object memory */
		if (__dispose) 
			__dispose(node->reference);
		/* second delete the link itself */
		cl->__dealloc(node);
	}
	
	/* init the sentinel again, since we do not unlink the deleted nodes */
	list_init(sent);

	cl->size = 0;
}

inline int r_dlist_size(struct r_dlist* cl) {
	return cl->size;
}

inline void cntr_list_add(struct r_dlist* cl, void* object) {
	r_dlist_add_back(cl, object);
}

inline struct r_dlist_node* r_dlist_find(struct r_dlist* cl, void* object) {
	struct list_link* link = cl->sent.next;
	struct r_dlist_node* node = NULL;

	while (link != &cl->sent) {
		node = container_of(link, struct r_dlist_node, link);

		if (node->reference == object) {
			return node;
		}

		link = link->next;
	}

	return NULL;
}

inline struct r_dlist_node* r_dlist_first(struct r_dlist* cl) {
	struct r_dlist_node* node = NULL;

	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		return NULL;
	}

	node = container_of(cl->sent.next, struct r_dlist_node, link);
	return node;
}
inline struct r_dlist_node* r_dlist_last(struct r_dlist* cl) {
	struct r_dlist_node* node = NULL;

	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		return NULL;
	}

	node = container_of(cl->sent.prev, struct r_dlist_node, link);
	return node;
}

inline void* r_dlist_remove(struct r_dlist* cl, struct r_dlist_node* node) {
	void* obj_ref = node->reference;

	list_unlink(&node->link);
	cl->__dealloc(node);

	cl->size --;

	return obj_ref;
}

inline void r_dlist_insert_before(struct r_dlist* cl, struct r_dlist_node* node, void* n_ref) {
	struct r_dlist_node* n_node = (struct r_dlist_node*)cl->__alloc(sizeof(struct r_dlist_node));

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	cl->size ++;
}

inline void r_dlist_insert_after(struct r_dlist* cl, struct r_dlist_node* node, void* n_ref) {
	struct r_dlist_node* n_node = (struct r_dlist_node*)cl->__alloc(sizeof(struct r_dlist_node));

	n_node->reference = n_ref;
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	cl->size ++;
}

inline void* r_dlist_front(struct r_dlist* cl) {
	struct r_dlist_node* node = r_dlist_first(cl);

	if (node) {
		return node->reference;
	}

	dbg_assert(cl->size == 0);
	return NULL;
}

inline void* r_dlist_back(struct r_dlist* cl) {
	struct r_dlist_node* node = r_dlist_last(cl);

	if (node) {
		return node->reference;
	}

	dbg_assert(cl->size == 0);
	return NULL;
}

inline void r_dlist_add_front(struct r_dlist* cl, void* object) {
	struct r_dlist_node* n_node = (struct r_dlist_node*)cl->__alloc(sizeof(struct r_dlist_node));

	n_node->reference = object;
	list_insert_front(&cl->sent, &n_node->link);

	cl->size ++;
}

inline void r_dlist_add_back(struct r_dlist* cl, void* object) {
	struct r_dlist_node* n_node = (struct r_dlist_node*)cl->__alloc(sizeof(struct r_dlist_node));

	n_node->reference = object;
	list_insert_back(&cl->sent, &n_node->link);

	cl->size ++;
}

inline void* r_dlist_remove_front(struct r_dlist* cl) {
	if (cl->size > 0) {
		struct list_link* link    = cl->sent.next;
		struct r_dlist_node* node = container_of(link, struct r_dlist_node, link);
		void*  object_ref         = node->reference;

		dbg_assert(link != &cl->sent);

		list_unlink(link);
		cl->__dealloc(node);

		cl->size --;

		return object_ref;
	}

	dbg_assert(cl->size == 0);
	dbg_assert(list_empty(&cl->sent));

	dbg_assert(false);

	return NULL;
}

inline void* r_dlist_remove_back(struct r_dlist* cl) {
	if (cl->size > 0) {
		struct list_link* link    = cl->sent.prev;
		struct r_dlist_node* node = container_of(link, struct r_dlist_node, link);
		void*  object_ref         = node->reference;

		list_unlink(link);
		cl->__dealloc(node);

		cl->size --;

		return object_ref;
	}

	dbg_assert(cl->size == 0);
	dbg_assert(list_empty(&cl->sent));

	dbg_assert(false);

	return NULL;
}
