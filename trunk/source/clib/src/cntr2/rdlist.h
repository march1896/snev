#ifndef _REF_DOUBLE_LINKED_LIST_H_
#define _REF_DOUBLE_LINKED_LIST_H_

/* we need the prototype of pf_alloc/pf_dealloc */
#include <imemmgr.h>
#include <list_link.h>

struct r_dlist_node {
	struct list_link     link;
	void*                reference;
};

struct r_dlist {
	/* alloc/dealloc manages the inner memory use of list_node*/
	pf_alloc             __alloc;
	pf_dealloc           __dealloc;

	int                  size;
	struct list_link     sent; /* sentinel */
};

extern inline void              r_dlist_init       (struct r_dlist* cl, pf_alloc __alloc, pf_dealloc __dealloc);
extern inline void              r_dlist_clear      (struct r_dlist* cl, pf_dispose __dispose);
extern inline int               r_dlist_size       (struct r_dlist* cl);
extern inline void              cntr_list_add        (struct r_dlist* cl, void* object);
extern inline struct r_dlist_node* r_dlist_find  (struct r_dlist* cl, void* object);
extern inline struct r_dlist_node* r_dlist_first (struct r_dlist* cl);
extern inline struct r_dlist_node* r_dlist_last  (struct r_dlist* cl);
extern inline void*             r_dlist_remove     (struct r_dlist* cl, struct r_dlist_node* node);
extern inline void              r_dlist_insert_before(struct r_dlist* cl, struct r_dlist_node* node, void* n_ref);
extern inline void              r_dlist_insert_after (struct r_dlist* cl, struct r_dlist_node* node, void* n_ref);
extern inline void*             r_dlist_front      (struct r_dlist* cl);
extern inline void*             r_dlist_back       (struct r_dlist* cl);
extern inline void              r_dlist_add_front  (struct r_dlist* cl, void* object);
extern inline void              r_dlist_add_back   (struct r_dlist* cl, void* object);
extern inline void*             r_dlist_remove_front(struct r_dlist* cl);
extern inline void*             r_dlist_remove_back(struct r_dlist* cl);

#endif /* _REF_DOUBLE_LINKED_LIST_H_ */
