#include <cntr_algorithm.h>
#include <cntr.h>
#include <citer_base.local.h>
#include <citer_cont.local.h>

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

	for (; !citer_equal(itr, end); citer_to_next(itr)) {
		proc(itr);
	}

	proc(itr);
}

void citer_for_each_v(citer begin, citer end, pf_for_each_process_v proc, void* param) {
	citer_dos(itr, begin);
	for (; !citer_equal(itr, end); citer_to_next(itr)) {
		proc(itr, param);
	}
	proc(itr, param);
}


bool cntr_equal(cntr first, cntr second) {
	bool equals = true;
	citer_dos(fc, NULL);
	citer_dos(sc, NULL);

	// TODO:
	return true;
}

#include <time.h>
#include <stdlib.h>
static void __randomize(citer begin, citer end) {
	if (citer_check_attr(begin, CITER_ATTR_CONT)) {
		int dis = citer_dis(begin, end);
		citer_dos(ran, begin);

		int x = ((rand() % dis) + dis) % dis;

		citer_move_n(ran, x);
		citer_swap(ran, end);
	}
}

static void __partition(citer begin, citer end, citer out, pf_compare_object comp) {
	citer_dos(posed, begin);
	citer_dos(fwd, begin);

	dbg_assert(!citer_equal(begin, end));

	__randomize(begin, end);

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

	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

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

static void __array_middle(citer begin, citer end, citer mid) {
	int dis = citer_dis(begin, end);
	int half = dis / 2;

	citer_assign(mid, begin);

	citer_move_n(mid, half);
}

static void __list_middle(citer begin, citer end, citer mid) {
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

static void citer_middle(citer begin, citer end, citer mid) {
	if (!citer_check_attr(begin, CITER_ATTR_CONT)) {
		__list_middle(begin, end, mid);
	}
	else {
		__array_middle(begin, end, mid);
	}
}

static void __merge_sort_copyback(citer itr, void* param) {
	citer to_copy = (citer)param;

	citer_set_ref(to_copy, citer_get_ref(itr));
	citer_to_next(to_copy);
}

/*
 * For array, we could easily divide the array into two, but we need to another O(n) space.
 * For list, we could easily do the merge without O(n) space, but need O(n) time to divide the list.
 */
void merge_sort(citer _begin, citer _end, pf_compare_object comp) {
	citer_dos(begin, _begin);
	citer_dos(end, _end);
	citer_dos(mid, NULL);
	citer_dos(pre_mid, NULL);
	cntr helper = cntr_create_as_array();

	if (citer_equal(begin, end)) return;

	citer_middle(begin, end, mid);
	citer_assign(pre_mid, mid);
	citer_to_prev(pre_mid);

	merge_sort(begin, pre_mid, comp);
	merge_sort(mid, end, comp);

	/* merge two parts */
	{
		bool fpo = false;  /* first part over */
		bool spo = false;  /* second part over */
		while (!fpo || !spo) {
			if (fpo) {
				/* process second half part */
				while (!citer_equal(mid, end)) {
					cntr_add_back(helper, citer_get_ref(mid));
					citer_to_next(mid);
				}
				cntr_add_back(helper, citer_get_ref(mid));
				spo = true;
			}
			else if (spo) {
				while (!citer_equal(begin, pre_mid)) {
					cntr_add_back(helper, citer_get_ref(begin));
					citer_to_next(begin);
				}
				cntr_add_back(helper, citer_get_ref(begin));
				fpo = true;
			}
			else {
				void* f_ref = citer_get_ref(begin);
				void* s_ref = citer_get_ref(mid);

				if (comp(f_ref, s_ref) <= 0) {
					cntr_add_back(helper, citer_get_ref(begin));
					if (citer_equal(begin, pre_mid)) fpo = true;
					else citer_to_next(begin);
				}
				else {
					cntr_add_back(helper, citer_get_ref(mid));
					if (citer_equal(mid, end)) spo = true;
					else citer_to_next(mid);
				}
			}
		}
	}

	/* copy back the merged result back to the original container */
	{
		citer_dos(helper_begin, NULL);
		citer_dos(helper_end, NULL);

		citer_dos(orig_begin, _begin);

		cntr_citer_begin(helper, helper_begin);
		cntr_citer_end(helper, helper_end);

		citer_for_each_v(helper_begin, helper_end, __merge_sort_copyback, orig_begin);
	}

	cntr_destroy(helper);
	return;
}

void reverse(citer begin, citer end) {
	citer_dos(first, begin);
	citer_dos(last, end);

	while (!citer_equal(first, last)) {
		citer_swap(first, last);

		citer_to_next(first);
		if (citer_equal(first, last)) break;
		citer_to_prev(last);
	}
}

bool prev_permutation(citer begin, citer end, pf_compare_object comp) {
	citer_dos(bwd, end);
	citer_dos(next, end);

	while (!citer_equal(next, begin)) {
		citer_to_prev(bwd);

		if (comp(citer_get_ref(bwd), citer_get_ref(next)) < 0) {
			citer_dos(sub, end);

			while (comp(citer_get_ref(sub), citer_get_ref(bwd)) >= 0)
				citer_to_prev(sub);

			citer_swap(bwd, sub);

			reverse(next, end);

			return true;
		}

		citer_to_prev(next);
	}

	return false;
}

bool next_permutation(citer begin, citer end, pf_compare_object comp) {
	citer_dos(bwd, end);
	citer_dos(next, end);

	while (!citer_equal(next, begin)) {
		citer_to_prev(bwd);
		if (comp(citer_get_ref(bwd), citer_get_ref(next)) > 0) {
			citer_dos(sub, end);

			/* maybe binary search */
			while (comp(citer_get_ref(sub), citer_get_ref(bwd)) <= 0)
				citer_to_prev(sub);

			citer_swap(bwd, sub);

			reverse(next, end);

			return true;
		}
		citer_to_prev(next);
	}

	return false;
}

bool find_first(citer begin, citer end, citer result, pf_find_accept accept) {
	return false;
}

bool find_last(citer begin, citer end, citer result, pf_find_accept accept) {
	return false;
}

/* 
 * No clear way to compare the target object to the comtainer object
 */
/*
citer binary_search(citer begin, citer end, citer result, void *target, pf_compare_object comp) {
	if (!citer_check_attr(begin, CITER_ATTR_CONT)) 
		return NULL;

	{
		citer_dos(first, begin);
		citer_dos(last, end);
		citer_dos(mid, NULL);

		int comp_res = 0;

		while (citer_dis(first, last) > 0) {
			__array_middle(first, last, mid);

			comp_res = comp(mid, target);

			if (comp_res) return 
		}
	}
}
*/

