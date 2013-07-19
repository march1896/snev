#include <list_link.h>

void list_init(struct list_link* sent) {
	sent->prev = sent;
	sent->next = sent;
}

bool list_empty(struct list_link* sent) {
	if (sent->prev == sent) {
		dbg_assert(sent->next == sent);
		return true;
	}

	dbg_assert(sent->next != sent);
	return false;
}

void list_link(struct list_link* n, struct list_link* prev, struct list_link *next) {
	n->prev = prev;
	n->next = next;

	prev->next = n;
	next->prev = n;
}

void list_unlink(struct list_link* n) {
	struct list_link* prev = n->prev;
	struct list_link* next = n->next;
	prev->next = next;
	next->prev = prev;
}

void list_insert_front(struct list_link* sent, struct list_link* n_node) {
	list_link(n_node, sent, sent->next);
}

void list_remove_front(struct list_link* sent) {
	list_unlink(sent->next);
}

void list_insert_back(struct list_link* sent, struct list_link* n_node) {
	list_link(n_node, sent->prev, sent);
}

void list_remove_back(struct list_link* sent) {
	list_unlink(sent->prev);
}

void list_remove(struct list_link* sent, struct list_link *node) {
	dbg_assert(sent != node);

	list_unlink(node);
}
