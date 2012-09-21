#include <block_link.h>
#include <list_link.h>

/**********************************************************************************/

struct list_link *blink_inc_find_ff (struct list_link *phead, int req) {
	struct list_link *itr = phead;
	while (itr != NULL) {
		if (block_data_size_from_blick(itr) >= req) 
			return itr;

		itr = itr->next;
	}

	return NULL;
}

struct list_link *blink_inc_find_bf (struct list_link *phead, int req) {
	return blink_inc_find_ff(phead, req);
}

void   blink_inc_pop  (struct list_link **pphead, struct list_link *pbd) {
	list_remove_h(pphead, pbd);
}

void   blink_inc_push (struct list_link **pphead, struct list_link *pbd) {
	struct list_link* next = *pphead;
	struct list_link* prev = NULL;
	int sz = block_data_size_from_blick(pbd);

	if (*pphead == NULL) {
		*pphead = pbd;
		return;
	}

	while (next != NULL) {
		if (block_data_size_from_blick(next) >= sz)
			break;

		prev = next;
		next = next->next;
	}

	list_link(pbd, prev, next);
	return;
}

struct list_link *blink_rnd_find_ff (struct list_link *phead, int req) {
	struct list_link *itr = phead;
	while (itr != NULL) {
		if (block_data_size_from_blick(itr) >= req) 
			return itr;

		itr = itr->next;
	}

	return NULL;
}
struct list_link *blink_rnd_find_bf (struct list_link *phead, int req) {
	struct list_link *best = NULL;
	struct list_link *itr = phead;
	int bsz = -1;

	while (itr != NULL) {
		int sz = block_data_size_from_blick(itr);

		if (sz >= req) {
			if (best == NULL || sz < bsz) {
				bsz = sz;
				best = itr;
			}
		}

		itr = itr->next;
	}

	return best;
}
void blink_rnd_pop  (struct list_link **pphead, struct list_link *pbd) {
	list_remove_h(pphead, pbd);
}
void blink_rnd_push (struct list_link **pphead, struct list_link *pbd) {
	list_insert_h(pphead, pbd);
}