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

	while (citer_valid(begin)) {
		citer* itr = begin;
		citer_to_next(itr);
		while (citer_valid(itr)) {
			if (comp(citer_get_ref(), citer_get_ref())) {
			}
		}
		citer_to_next(begin);
	}
}

typedef bool (*pf_find_accept)(void* object, void* param);
typedef cntr_find_criterion {
	pf_find_accept accept;
	void*          param;
} cntr_fc;

bool find_first(citer* begin, citer* end, citer* result, cntr_fc* criterion) {
	return false;
}

bool find_last(citer* begin, citer* end, citer* result, cntr_fc* criterion) {
	return false;
}

