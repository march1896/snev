#include <util/list_link.h>

inline void list_init(struct listlink* sent) {
	sent->prev = sent;
	sent->next = sent;
}

inline bool list_empty(struct listlink* sent) {
	if (sent->prev == sent) {
		dbg_assert(sent->next == sent);
		return true;
	}

	dbg_assert(sent->next != sent);
	return false;
}

inline void list_link(struct listlink* n, struct listlink* prev, struct listlink *next) {
	n->prev = prev;
	n->next = next;

	prev->next = n;
	next->prev = n;
}

inline void list_unlink(struct listlink* n) {
	struct listlink* prev = n->prev;
	struct listlink* next = n->next;
	prev->next = next;
	next->prev = prev;
}

inline void list_insert_front(struct listlink* sent, struct listlink* n_node) {
	list_link(n_node, sent, sent->next);
}

inline void list_remove_front(struct listlink* sent) {
	list_unlink(sent->next);
}

inline void list_insert_back(struct listlink* sent, struct listlink* n_node) {
	list_link(n_node, sent->prev, sent);
}

inline void list_remove_back(struct listlink* sent) {
	list_unlink(sent->prev);
}

inline void list_remove(struct listlink* sent, struct listlink *node) {
	dbg_assert(sent != node);

	list_unlink(node);
}

inline void list_foreach(struct listlink* sent, pf_list_link_process process) {
	struct listlink* next = sent->next;
	struct listlink* prev = NULL;

	while (next != sent) {
		prev = next;
		next = next->next;

		/* we must first move the link to next, then do the processing, because 
		 * the process callback may delete the link itself */
		process(prev);
	}
}

void list_foreach_v(struct listlink* sent, pf_list_link_process_v process_v, void* param) {
	struct listlink* next = sent->next;
	struct listlink* prev = NULL;

	while (next != sent) {
		prev = next;
		next = next->next;

		/* we must first move the link to next, then do the processing, because 
		 * the process callback may delete the link itself */
		process_v(prev, param);
	}
}
