#ifndef _LIST_DOUBLE_LINK_H_
#define _LIST_DOUBLE_LINK_H_

#include <cominc.h>

struct list_link {
	struct list_link *prev;
	struct list_link *next;
};

void list_insert_h(struct list_link **head, struct list_link *n_node);

void list_remove_h(struct list_link **head, struct list_link *n_node);

void list_insert_front_ht(struct list_link **head, struct list_link **tail, struct list_link *n_node);

void list_insert_back_ht(struct list_link **head, struct list_link **tail, struct list_link *n_node);

void list_remove_ht(struct list_link **head, struct list_link **tail, struct list_link *n_node);

#endif /* _LIST_DOUBLE_LINK_H_ */
