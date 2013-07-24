#ifndef _CNTR_LIST_REF_H_
#define _CNTR_LIST_REF_H_

/* TODO: should I rename this file to raw_list, list_ref, or something, cntr_list is too common */

/* we need the prototype of pf_alloc/pf_dealloc */
#include <heap_def.h>
#include <list_link.h>

struct list_node {
	struct list_link     link;
	void*                object;
};

struct cntr_list {
	/* alloc/dealloc manages the inner memory use of list_node*/
	pf_alloc             alloc;
	pf_dealloc           dealloc;

	int                  size;
	struct list_link     sent; /* sentinel */
};

/* TODO: move this to somewhere */
typedef void (*pf_obj_dispose)(void* buff);

extern inline void              cntr_list_init       (struct cntr_list* cl, pf_alloc __alloc, pf_dealloc __dealloc);
extern inline void              cntr_list_clear      (struct cntr_list* cl, pf_obj_dispose __dispose);
extern inline int               cntr_list_size       (struct cntr_list* cl);
extern inline void              cntr_list_add        (struct cntr_list* cl, void* object);
extern inline struct list_node* cntr_list_find       (struct cntr_list* cl, void* object);
extern inline struct list_node* cntr_list_first      (struct cntr_list* cl);
extern inline struct list_node* cntr_list_last       (struct cntr_list* cl);
extern inline void*             cntr_list_front      (struct cntr_list* cl);
extern inline void*             cntr_list_back       (struct cntr_list* cl);
extern inline void              cntr_list_add_front  (struct cntr_list* cl, void* object);
extern inline void              cntr_list_add_back   (struct cntr_list* cl, void* object);
extern inline void*             cntr_list_remove_front(struct cntr_list* cl);
extern inline void*             cntr_list_remove_back(struct cntr_list* cl);

#endif /* _CNTR_LIST_REF_H_ */
