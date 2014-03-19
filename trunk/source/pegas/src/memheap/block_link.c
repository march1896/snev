#include <memheap/block_link.h>
#include <util/list_link.h>

/**********************************************************************************/

struct listlink *blink_inc_find_ff (struct listlink* psent, int req) {
	struct listlink *itr = psent->next;
	while (itr != psent) {
		if (block_data_size_from_blick(itr) >= req) 
			return itr;

		itr = itr->next;
	}

	return NULL;
}

struct listlink *blink_inc_find_bf (struct listlink* psent, int req) {
	return blink_inc_find_ff(psent, req);
}

void   blink_inc_pop  (struct listlink* psent, struct listlink *pbd) {
	list_remove(psent, pbd);
}

void   blink_inc_push (struct listlink* psent, struct listlink *pbd) {
	if (list_empty(psent)) {
		list_insert_front(psent, pbd);
	}
	else {
		struct listlink* next = psent->next;
		struct listlink* prev = NULL;
		int sz = block_data_size_from_blick(pbd);

		while (next != psent) {
			if (block_data_size_from_blick(next) >= sz)
				break;

			prev = next;
			next = next->next;
		}

		list_link(pbd, prev, next);
	}

	return;
}

struct listlink *blink_rnd_find_ff (struct listlink* psent, int req) {
	return blink_inc_find_ff(psent, req);
}
struct listlink *blink_rnd_find_bf (struct listlink* psent, int req) {
	struct listlink *best = NULL;
	struct listlink *itr = psent->next;
	int bsz = -1;

	while (itr != psent) {
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
void blink_rnd_pop  (struct listlink* psent, struct listlink *pbd) {
	list_remove(psent, pbd);
}
void blink_rnd_push (struct listlink* psent, struct listlink *pbd) {
	list_insert_front(psent, pbd);
}