#include <cominc.h>
#include <cntr_algorithm.h>
#include <cntr_iterator.h>

void swap(citer* first, citer* second) {
	void *first_ref = citer_get_ref(first);
	citer_set_ref(first, citer_get_ref(second));
	citer_set_ref(second, first_ref);
}

typedef int (*pf_compare_object)(void*, void*);
void quick_sort(citer* begin, citer* end, pf_compare_object comp) {
	
}

void bubble_sort(citer* begin, citer* end, pf_compare_object comp) {
	dbg_assert(comp != NULL);
	if (begin == end) return;

	while (true) {
		bool swaped = false;
		citer prev = *begin;
		citer next = prev;
		assert(citer_valid(&prev));
		citer_to_next(&next);
		while (citer_valid(&next)) {
			if (comp(citer_get_ref(&prev), citer_get_ref(&next)) > 0) {
				swap(&prev, &next);
				swaped = true;
			}
			prev = next;
			citer_to_next(&next);
		}
		if (swaped == false) break;
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

