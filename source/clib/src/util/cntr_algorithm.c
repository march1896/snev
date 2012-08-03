#include <cominc.h>
#include <cntr_algorithm.h>
#include <cntr_iterator.h>
#include <cntr_iterator.local.h>

void citer_swap(citer first, citer second) {
	void *first_ref = citer_get_ref(first);

	if (citer_equal(first, second)) return;

	citer_set_ref(first, citer_get_ref(second));
	citer_set_ref(second, first_ref);
}

inline bool citer_equal(citer first, citer second) {
	cntr_iterator* lhs = (cntr_iterator*)first;
	cntr_iterator* rhs = (cntr_iterator*)second;

	if (lhs->connection == rhs->connection) {
		// dbg_assert(lhs->container == rhs->container && lhs->ops == rhs->ops);
		return true;
	}
	return false;
}

inline void citer_assign(citer to, citer from) {
	cntr_iterator* lhs = (cntr_iterator*) to;
	cntr_iterator* rhs = (cntr_iterator*) from;

	lhs->connection = rhs->connection;
	lhs->__vt = rhs->__vt;
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
		dbg_assert(citer_valid(fwd));
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

	dbg_assert(comp != NULL && citer_valid(begin) && citer_valid(end));

	if (citer_equal(first, last)) return;
	while (true) {
		citer_dos(prev, first);
		citer_dos(next, first);
		citer_dos(last_swaped, first);

		do {
			citer_to_next(next);
			dbg_assert(citer_valid(next));

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

typedef bool (*pf_find_accept)(void* object, void* param);

bool find_first(citer begin, citer end, citer result, cntr_fc* criterion) {
	return false;
}

bool find_last(citer begin, citer end, citer result, cntr_fc* criterion) {
	return false;
}

