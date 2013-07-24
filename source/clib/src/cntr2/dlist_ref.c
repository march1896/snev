#include <dlist_ref.h>

#include <util/list_link.h>

inline void dlist_ref_init(struct dlist_ref* cl, pf_alloc __alloc, pf_dealloc __dealloc) {
	dbg_assert(__alloc   != NULL);
	dbg_assert(__dealloc != NULL);

	cl->alloc     = __alloc;
	cl->dealloc   = __dealloc;
	cl->size      = 0;

	list_init(&cl->sent);
}

inline void dlist_ref_clear(struct dlist_ref* cl, pf_dispose __dispose) {
	struct list_link* sent = &cl->sent;
	struct list_link* next = sent->next;
	struct dlist_ref_node* node = NULL;

	dbg_assert(sent != NULL);

	while (next != sent) {
		node = container_of(next, struct dlist_ref_node, link);

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

inline int dlist_ref_size(struct dlist_ref* cl) {
	return cl->size;
}

inline void cntr_list_add(struct dlist_ref* cl, void* object) {
	dlist_ref_add_back(cl, object);
}

inline struct dlist_ref_node* dlist_ref_find(struct dlist_ref* cl, void* object) {
	struct list_link* link = cl->sent.next;
	struct dlist_ref_node* node = NULL;

	while (link != &cl->sent) {
		node = container_of(link, struct dlist_ref_node, link);

		if (node->object == object) {
			return node;
		}

		link = link->next;
	}

	return NULL;
}

inline struct dlist_ref_node* dlist_ref_first(struct dlist_ref* cl) {
	struct dlist_ref_node* node = NULL;

	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		return NULL;
	}

	node = container_of(cl->sent.next, struct dlist_ref_node, link);
	return node;
}
inline struct dlist_ref_node* dlist_ref_last(struct dlist_ref* cl) {
	struct dlist_ref_node* node = NULL;

	if (cl->size == 0) {
		dbg_assert(list_empty(&cl->sent));

		return NULL;
	}

	node = container_of(cl->sent.prev, struct dlist_ref_node, link);
	return node;
}

inline void* dlist_ref_remove(struct dlist_ref* cl, struct dlist_ref_node* node) {
	void* obj_ref = node->object;

	list_unlink(&node->link);
	cl->dealloc(node);

	cl->size --;

	return obj_ref;
}

inline void dlist_ref_insert_before(struct dlist_ref* cl, struct dlist_ref_node* node, void* n_ref) {
	struct dlist_ref_node* n_node = cl->alloc(sizeof(struct dlist_ref_node));

	n_node->object = n_ref;
	list_link(&n_node->link, (&node->link)->prev, &node->link);

	cl->size ++;
}

inline void dlist_ref_insert_after(struct dlist_ref* cl, struct dlist_ref_node* node, void* n_ref) {
	struct dlist_ref_node* n_node = cl->alloc(sizeof(struct dlist_ref_node));

	n_node->object = n_ref;
	list_link(&n_node->link, (&node->link), (&node->link)->next);

	cl->size ++;
}

inline void* dlist_ref_front(struct dlist_ref* cl) {
	struct dlist_ref_node* node = dlist_ref_first(cl);

	if (node) {
		return node->object;
	}

	dbg_assert(cl->size == 0);
	return NULL;
}

inline void* dlist_ref_back(struct dlist_ref* cl) {
	struct dlist_ref_node* node = dlist_ref_last(cl);

	if (node) {
		return node->object;
	}

	dbg_assert(cl->size == 0);
	return NULL;
}

inline void dlist_ref_add_front(struct dlist_ref* cl, void* object) {
	struct dlist_ref_node* n_node = cl->alloc(sizeof(struct dlist_ref_node));

	n_node->object = object;
	list_insert_front(&cl->sent, &n_node->link);

	cl->size ++;
}

inline void dlist_ref_add_back(struct dlist_ref* cl, void* object) {
	struct dlist_ref_node* n_node = cl->alloc(sizeof(struct dlist_ref_node));

	n_node->object = object;
	list_insert_back(&cl->sent, &n_node->link);

	cl->size ++;
}

inline void* dlist_ref_remove_front(struct dlist_ref* cl) {
	if (cl->size > 0) {
		struct list_link* link = cl->sent.next;
		struct dlist_ref_node* node = container_of(link, struct dlist_ref_node, link);
		void*  object_ref      = node->object;

		list_unlink(link);
		cl->dealloc(node);

		cl->size --;

		return object_ref;
	}

	dbg_assert(cl->size == 0);
	dbg_assert(list_empty(&cl->sent));

	dbg_assert(false);

	return NULL;
}

inline void* dlist_ref_remove_back(struct dlist_ref* cl) {
	if (cl->size > 0) {
		struct list_link* link = cl->sent.prev;
		struct dlist_ref_node* node = container_of(link, struct dlist_ref_node, link);
		void*  object_ref      = node->object;

		list_unlink(link);
		cl->dealloc(node);

		cl->size --;

		return object_ref;
	}

	dbg_assert(cl->size == 0);
	dbg_assert(list_empty(&cl->sent));

	dbg_assert(false);

	return NULL;
}
