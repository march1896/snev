#include <list_link.h>

void list_link(struct list_link *n, struct list_link *prev, struct list_link *next) {
	n->prev = prev;
	n->next = next;

	if (prev) {
		dbg_assert(prev->next == next);
		prev->next = n;
	}
	if (next) {
		dbg_assert(next->prev == prev);
		next->prev = n;
	}
}

void list_unnlink(struct list_link *n) {
	struct list_link *prev = n->prev;
	struct list_link *next = n->next;

	if (prev)
		prev->next = next;
	if (next) 
		next->prev = prev;

	n->prev = n->next = NULL;
}

void list_insert_h(struct list_link **head, struct list_link *n_node) {
	list_link(n_node, NULL, *head);

	*head = n_node;
}

void list_remove_h(struct list_link **head, struct list_link *n_node) {
	if (*head == n_node) {
		dbg_assert(n_node->prev == NULL);
		*head = n_node->next;
	}

	list_unnlink(n_node);
}

void list_insert_front_ht(struct list_link **head, struct list_link **tail, struct list_link *n_node) {
	list_link(n_node, NULL, *head);
		
	*head = n_node;
	if (*tail == NULL) 
		*tail = n_node;
}

void list_insert_back_ht(struct list_link **head, struct list_link **tail, struct list_link *n_node) {
	list_link(n_node, *tail, NULL);

	*tail = n_node;

	if (*head == NULL)
		*head = n_node;
}

void list_remove_ht(struct list_link **head, struct list_link **tail, struct list_link *n_node) {
	if (*head == n_node) {
		dbg_assert(n_node->prev == NULL);
		*head = n_node->next;
	}

	if (*tail == n_node) {
		dbg_assert(n_node->next == NULL);
		*tail = n_node->prev;
	}

	list_unnlink(n_node);
}
