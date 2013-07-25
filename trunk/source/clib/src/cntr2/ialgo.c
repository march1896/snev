#include <ialgo.h>
#include <iitr.h>
#include <ifactory.h>

typedef void (*pf_ref_process)  (void* __ref);
typedef void (*pf_ref_process_v)(void* __ref, void* param);
typedef bool (*pf_ref_compare)  (void* ref_a, void* ref_b);

void foreach(itrfwd begin, itrfwd end, pf_ref_process cb) {
	/* get interface of existing object, we do not create any object on heap */
	dbg_assert(is_itrfwd(begin));
	dbg_assert(is_itrfwd(end));

	/* if a common iterator has passed in, assuming it at least has a forward interface */
	dbg_assert(begin != NULL && end != NULL);

	while (!itr_equals(begin, end)) {
		cb(itr_get_ref(begin));
		itr_to_next(begin);
	}

	return;
}

void foreach_v(itrfwd begin, itrfwd end, pf_ref_process_v cb, void* param) {
	dbg_assert(is_itrfwd(begin));
	dbg_assert(is_itrfwd(end));

	while (!itr_equals(begin, end)) {
		cb(itr_get_ref(begin), param);
		itr_to_next(begin);
	}

	return;
}

void sort_b(itrbid begin, itrbid end, pf_ref_compare comp) {
	dbg_assert(is_itrbid(begin));
	dbg_assert(is_itrbid(end));
}
void sort_r(itrrac begin, itrrac end, pf_ref_compare comp) {
	dbg_assert(is_itrrac(begin));
	dbg_assert(is_itrrac(end));
}

/* TODO: should we consider single-linked-list, only forward iterators? */
void reverse_b(itrbid begin, itrbid end) {
	dbg_assert(is_itrbid(begin));
	dbg_assert(is_itrbid(end));

	if (itr_equals(begin, end)) {
		return;
	}

	itr_to_prev(end);
	while (!itr_equals(begin, end)) {
		void* b_ref = itr_get_ref(begin);
		void* e_ref = itr_get_ref(end);
		
		itr_set_ref(begin, e_ref);
		itr_set_ref(end, b_ref);

		itr_to_next(begin);

		if (itr_equals(begin, end)) {
			break;
		}
		
		itr_to_prev(end);
	}
}

void prev_permutation_b(itrbid begin, itrbid end) {
	dbg_assert(is_itrbid(begin));
	dbg_assert(is_itrbid(end));
}

void next_permutation_b(itrbid begin, itrbid end) {
	dbg_assert(is_itrbid(begin));
	dbg_assert(is_itrbid(end));
}
