#include <cominc.h>
#include <cntr_algorithm.h>
#include <cntr_iterator.h>

void citer_swap(citer* first, citer* second) {
	void *first_ref = citer_get_ref(first);
	citer_set_ref(first, citer_get_ref(second));
	citer_set_ref(second, first_ref);
}

typedef int (*pf_compare_object)(void*, void*);
void quick_sort(citer* begin, citer* end, pf_compare_object comp) {
	
}

void bubble_sort(citer* begin, citer* end, pf_compare_object comp) {
	citer first = *begin;
	citer last = *end;

	dbg_assert(comp != NULL && citer_valid(begin) && citer_valid(end));

	if (citer_equal(&first, &last)) return;
	while (true) {
		citer prev, next, last_swaped;
		next = prev = last_swaped = first;

		do {
			citer_to_next(&next);
			assert(citer_valid(&next));

			if (comp(citer_get_ref(&prev), citer_get_ref(&next)) > 0) {
				citer_swap(&prev, &next);

				last_swaped = prev;
			}
			prev = next;
		} while (!citer_equal(&next, &last));

		if (citer_equal(&last_swaped, &first)) break;
		else last = last_swaped;
	}
	return;
}

typedef bool (*pf_find_accept)(void* object, void* param);

bool find_first(citer* begin, citer* end, citer* result, cntr_fc* criterion) {
	return false;
}

bool find_last(citer* begin, citer* end, citer* result, cntr_fc* criterion) {
	return false;
}

