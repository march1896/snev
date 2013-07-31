#include <list_link.h>

inline void list_init(struct list_link* sent) {
	sent->prev = sent;
	sent->next = sent;
}

inline bool list_empty(struct list_link* sent) {
	if (sent->prev == sent) {
		dbg_assert(sent->next == sent);
		return true;
	}

	dbg_assert(sent->next != sent);
	return false;
}

inline void list_link(struct list_link* n, struct list_link* prev, struct list_link *next) {
	n->prev = prev;
	n->next = next;

	prev->next = n;
	next->prev = n;
}

inline void list_unlink(struct list_link* n) {
	struct list_link* prev = n->prev;
	struct list_link* next = n->next;
	prev->next = next;
	next->prev = prev;
}

inline void list_insert_front(struct list_link* sent, struct list_link* n_node) {
	list_link(n_node, sent, sent->next);
}

inline void list_remove_front(struct list_link* sent) {
	list_unlink(sent->next);
}

inline void list_insert_back(struct list_link* sent, struct list_link* n_node) {
	list_link(n_node, sent->prev, sent);
}

inline void list_remove_back(struct list_link* sent) {
	list_unlink(sent->prev);
}

inline void list_remove(struct list_link* sent, struct list_link *node) {
	dbg_assert(sent != node);

	list_unlink(node);
}

inline void list_foreach(struct list_link* sent, pf_list_link_process process) {
	struct list_link* next = sent->next;
	struct list_link* prev = NULL;

	while (next != sent) {
		prev = next;
		next = next->next;

		/* we must first move the link to next, then do the processing, because 
		 * the process callback may delete the link itself */
		process(prev);
	}
}

void list_foreach_v(struct list_link* sent, pf_list_link_process_v process_v, void* param) {
	struct list_link* next = sent->next;
	struct list_link* prev = NULL;

	while (next != sent) {
		prev = next;
		next = next->next;

		/* we must first move the link to next, then do the processing, because 
		 * the process callback may delete the link itself */
		process_v(prev, param);
	}
}
