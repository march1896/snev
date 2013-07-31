#include <ialgo.h>
#include <iitr.h>
#include <ifactory.h>

void foreach(itrfwd begin, itrfwd end, pf_ref_process cb) {
	/* get interface of existing object, we do not create any object on heap */
	dbg_assert(is_itrfwd(begin));
	dbg_assert(is_itrfwd(end));

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

static void itr_swap_ref(iterator itr_a, iterator itr_b) {
	void* ref_a = itr_get_ref(itr_a);
	void* ref_b = itr_get_ref(itr_b);

	itr_set_ref(itr_a, ref_b);
	itr_set_ref(itr_b, ref_a);
}

void reverse_b(itrbid begin, itrbid end) {
	dbg_assert(is_itrbid(begin));
	dbg_assert(is_itrbid(end));

	if (itr_equals(begin, end)) {
		return;
	}

	itr_to_prev(end);
	while (!itr_equals(begin, end)) {
		itr_swap_ref(begin, end);

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
