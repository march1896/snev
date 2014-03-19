#ifndef _LIST_DOUBLE_LINK_H_
#define _LIST_DOUBLE_LINK_H_

#include <util/cominc.h>

struct listlink {
	struct listlink *prev;
	struct listlink *next;
};

/* TODO: change list_link to list_link_push and list_unlink to list_link_pop */
extern inline void list_link(struct listlink *n, struct listlink *prev, struct listlink *next);

extern inline void list_unlink(struct listlink *n);

/* TODO: should the below interface renamed, should the link only deals with 'link', the following code deals 
 * with a common sentinel based, double linked list, but not the link */
extern inline void list_init(struct listlink* sent);

extern inline bool list_empty(struct listlink* sent);

/* This function insert a node in the front of the list, by the given sentinel */
/* TODO: Should we add another function named "insert_before", which insert a new node before the given
 * node, in the sentinel based list, this function will be exactly the same as list_insert_back */
extern inline void list_insert_front(struct listlink* sent, struct listlink* n_node);

extern inline void list_remove_front(struct listlink* sent);

extern inline void list_insert_back(struct listlink* sent, struct listlink* n_node);

extern inline void list_remove_back(struct listlink* sent);

extern inline void list_remove(struct listlink* sent, struct listlink *node);

typedef void (*pf_list_link_process)  (struct listlink* link);
typedef void (*pf_list_link_process_v)(struct listlink* link, void* param);
extern inline void list_foreach  (struct listlink* sent, pf_list_link_process process);
extern inline void list_foreach_v(struct listlink* sent, pf_list_link_process_v process_v, void* param);

#endif /* _LIST_DOUBLE_LINK_H_ */
