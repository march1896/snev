#ifndef _LIST_DOUBLE_LINK_H_
#define _LIST_DOUBLE_LINK_H_

#include <cominc.h>

struct list_link {
	struct list_link *prev;
	struct list_link *next;
};

extern inline void list_init(struct list_link* sent);

extern inline bool list_empty(struct list_link* sent);

extern inline void list_link(struct list_link *n, struct list_link *prev, struct list_link *next);

extern inline void list_unlink(struct list_link *n);

extern inline void list_insert_front(struct list_link* sent, struct list_link* n_node);

extern inline void list_remove_front(struct list_link* sent);

extern inline void list_insert_back(struct list_link* sent, struct list_link* n_node);

extern inline void list_remove_back(struct list_link* sent);

extern inline void list_remove(struct list_link* sent, struct list_link *node);

#endif /* _LIST_DOUBLE_LINK_H_ */
