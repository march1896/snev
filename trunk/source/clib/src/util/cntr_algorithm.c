#include <cntr_algorithm.h>
#include <citer_base.h>
#include <citer_base.local.h>

void citer_swap(citer first, citer second) {
	void *first_ref = citer_get_ref(first);

	if (citer_equal(first, second)) return;

	citer_set_ref(first, citer_get_ref(second));
	citer_set_ref(second, first_ref);
}

inline bool citer_equal(citer first, citer second) {
	citer_base* lhs = (citer_base*)first;
	citer_base* rhs = (citer_base*)second;

	if (lhs->connection == rhs->connection) {
		dbg_assert(lhs->__vt == rhs->__vt);
		return true;
	}
	return false;
}

inline void citer_assign(citer to, citer from) {
	citer_base* lhs = (citer_base*) to;
	citer_base* rhs = (citer_base*) from;

	lhs->connection = rhs->connection;
	lhs->__vt = rhs->__vt;
}

void citer_for_each(citer begin, citer end, pf_for_each_process proc) {
	citer_dos(itr, begin);
	bool over = false;

	for (; !citer_equal(itr, end); citer_to_next(itr)) {
		proc(itr);
	}

	proc(itr);
}


bool cntr_equal(cntr first, cntr second) {
	bool equals = true;
	citer_dos(fc, NULL);
	citer_dos(sc, NULL);

	// TODO:
	return true;
}

void __partition(citer begin, citer end, citer out, pf_compare_object comp) {
	citer_dos(posed, begin);
	citer_dos(fwd, begin);

	dbg_assert(!citer_equal(begin, end));

	while (!citer_equal(fwd, end)) {
		if (comp(citer_get_ref(fwd), citer_get_ref(end)) < 0) {
			citer_swap(posed, fwd);

			citer_to_next(posed);
		}

		citer_to_next(fwd);
	}

	citer_swap(posed, end);
	citer_assign(out, posed);
}

void quick_sort(citer begin, citer end, pf_compare_object comp) {
	citer_dos(mid, NULL);

	if (citer_equal(begin, end)) return;

	__partition(begin, end, mid, comp);

	if (!citer_equal(begin, mid)) {
		citer_dos(prev_mid, mid);
		citer_to_prev(prev_mid);

		quick_sort(begin, prev_mid, comp);
	}
	if (!citer_equal(mid, end)) {
		citer_dos(next_mid, mid);
		citer_to_next(next_mid);

		quick_sort(next_mid, end, comp);
	}

	return;
}

void bubble_sort(citer begin, citer end, pf_compare_object comp) {
	citer_dos(first, begin);
	citer_dos(last, end);

	dbg_assert(comp != NULL);

	if (citer_equal(first, last)) return;
	while (true) {
		citer_dos(prev, first);
		citer_dos(next, first);
		citer_dos(last_swaped, first);

		do {
			citer_to_next(next);

			if (comp(citer_get_ref(prev), citer_get_ref(next)) > 0) {
				citer_swap(prev, next);

				citer_assign(last_swaped, prev);
			}
			citer_assign(prev, next);
		} while (!citer_equal(next, last));

		if (citer_equal(last_swaped, first)) break;
		else citer_assign(last, last_swaped);
	}
	return;
}

static void __list_find_middle(const citer begin, const citer end, citer mid) {
	citer_dos(fast, begin);
	citer_dos(slow, begin);

	while (!citer_equal(fast, end)) {
		citer_to_next(slow);

		citer_to_next(fast);
		if (citer_equal(fast, end)) break;
		citer_to_next(fast);
	}
	citer_assign(mid, slow);
}

static void __list_merge_sort(citer begin, citer end, pf_compare_object comp) {
	citer_dos(mid, NULL);
	citer_dos(pre_mid, NULL);
	citer_dos(n_begin, NULL);

	if (citer_equal(begin, end)) return;

	__list_find_middle(begin, end, mid);
	citer_assign(pre_mid, mid);
	citer_to_prev(pre_mid);

	__list_merge_sort(begin, pre_mid, comp);
	__list_merge_sort(mid, end, comp);

	while (!citer_equal(begin, pre_mid) || !citer_equal(mid, end)) {
		
	}
}

/*
 * For array, we could easily divide the array into two, but we need to another O(n) space.
 * For list, we could easily do the merge without O(n) space, but need O(n) time to divide the list.
 */
void merge_sort(citer begin, citer end, pf_compare_object comp) {
	cattr attrib= citer_attrib(begin);
	
	dbg_assert(attrib == citer_attrib(end) && comp != NULL);

	if (citer_check_attr(attrib, CITER_ATTR_LINK) {
		__list_merge_sort(begin, end, comp);
	}
	else if (citer_check_attr(attrib, CITER_ATTR_CONT) {

	}
	else {
		dbg_assert(false);
	}
	return;
}

bool find_first(citer begin, citer end, citer result, pf_find_accept accept) {
	return false;
}

bool find_last(citer begin, citer end, citer result, pf_find_accept accept) {
	return false;
}

