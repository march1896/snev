#ifndef _LIST_DOUBLE_LINK_H_
#define _LIST_DOUBLE_LINK_H_

#include <cominc.h>

struct list_link {
	struct list_link *prev;
	struct list_link *next;
};

/* TODO: change list_link to list_link_push and list_unlink to list_link_pop */
extern inline void list_link(struct list_link *n, struct list_link *prev, struct list_link *next);

extern inline void list_unlink(struct list_link *n);

/* TODO: should the below interface renamed, should the link only deals with 'link', the following code deals 
 * with a common sentinel based, double linked list, but not the link */
extern inline void list_init(struct list_link* sent);

extern inline bool list_empty(struct list_link* sent);

/* This function insert a node in the front of the list, by the given sentinel */
/* TODO: Should we add another function named "insert_before", which insert a new node before the given
 * node, in the sentinel based list, this function will be exactly the same as list_insert_back */
extern inline void list_insert_front(struct list_link* sent, struct list_link* n_node);

extern inline void list_remove_front(struct list_link* sent);

extern inline void list_insert_back(struct list_link* sent, struct list_link* n_node);

extern inline void list_remove_back(struct list_link* sent);

extern inline void list_remove(struct list_link* sent, struct list_link *node);

typedef void (*pf_list_link_process)  (struct list_link* each);
typedef void (*pf_list_link_process_v)(struct list_link* each, void* param);
extern inline void list_foreach  (struct list_link* sent, pf_list_link_process process);
extern inline void list_foreach_v(struct list_link* sent, pf_list_link_process_v process_v, void* param);

#endif /* _LIST_DOUBLE_LINK_H_ */
