#ifndef _DOUBLE_LINKED_LIST_REF_H_
#define _DOUBLE_LINKED_LIST_REF_H_

/* we need the prototype of pf_alloc/pf_dealloc */
#include <imemmgr.h>
#include <list_link.h>

struct dlist_ref_node {
	struct list_link     link;
	void*                object;
};

struct dlist_ref {
	/* alloc/dealloc manages the inner memory use of list_node*/
	pf_alloc             alloc;
	pf_dealloc           dealloc;

	int                  size;
	struct list_link     sent; /* sentinel */
};

extern inline void              dlist_ref_init       (struct dlist_ref* cl, pf_alloc __alloc, pf_dealloc __dealloc);
extern inline void              dlist_ref_clear      (struct dlist_ref* cl, pf_dispose __dispose);
extern inline int               dlist_ref_size       (struct dlist_ref* cl);
extern inline void              cntr_list_add        (struct dlist_ref* cl, void* object);
extern inline struct dlist_ref_node* dlist_ref_find  (struct dlist_ref* cl, void* object);
extern inline struct dlist_ref_node* dlist_ref_first (struct dlist_ref* cl);
extern inline struct dlist_ref_node* dlist_ref_last  (struct dlist_ref* cl);
extern inline void*             dlist_ref_remove     (struct dlist_ref* cl, struct dlist_ref_node* node);
extern inline void              dlist_ref_insert_before(struct dlist_ref* cl, struct dlist_ref_node* node, void* n_ref);
extern inline void              dlist_ref_insert_after (struct dlist_ref* cl, struct dlist_ref_node* node, void* n_ref);
extern inline void*             dlist_ref_front      (struct dlist_ref* cl);
extern inline void*             dlist_ref_back       (struct dlist_ref* cl);
extern inline void              dlist_ref_add_front  (struct dlist_ref* cl, void* object);
extern inline void              dlist_ref_add_back   (struct dlist_ref* cl, void* object);
extern inline void*             dlist_ref_remove_front(struct dlist_ref* cl);
extern inline void*             dlist_ref_remove_back(struct dlist_ref* cl);

#endif /* _DOUBLE_LINKED_LIST_REF_H_ */
