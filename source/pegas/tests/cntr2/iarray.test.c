#include <cntr2/iarray.h>
#include <cntr2/iitr.h>
#include <cntr2/ifactory.h>
#include <memheap/heap_global.h>

#include "iarray.test.h"
#include "test_util.h"
#include "ele.data.h"

static void array_test_basic_itr_operation(_object* a, struct test_data_desc* td_desc) {
	address *test_data_addr   = td_desc->data_repo;
	unknown_traits *td_traits = td_desc->data_traits;
	iarray_clear(a);
	dbg_assert(iarray_empty(a));

	iarray_add_front(a, test_data_addr[4]);
	iarray_add_back (a, test_data_addr[5]);
	iarray_add_front(a, test_data_addr[3]);
	iarray_add_back (a, test_data_addr[6]);
	iarray_add_front(a, test_data_addr[2]);
	iarray_add_back (a, test_data_addr[7]);
	iarray_add_front(a, test_data_addr[1]);
	iarray_add_back (a, test_data_addr[8]);
	/* now the a contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	/* check is repo is destroyed */
	dbg_assert(iarray_size(a) == 8);
	{
		iterator itr = iarray_itr_create(a, itr_begin);
		const_iterator end = iarray_itr_end(a);
		intptr_t current = 1;
		const unknown* x = NULL;

		/* traverse the a */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			x = itr_get_ref(itr);
			/* instance is managed by the container */
			dbg_assert(x != test_data_addr[current]);
			dbg_assert(td_traits->__equals(x, test_data_addr[current]));
			current ++;
		}

		/* test itr_assign */
		iarray_itr_assign(a, itr, itr_begin);
		x = itr_get_ref(itr);
		dbg_assert(td_traits->__equals(x, test_data_addr[1]));
		iarray_itr_assign(a, itr, itr_end);
		itr_to_prev(itr);
		x = (int*)itr_get_ref(itr);
		dbg_assert(td_traits->__equals(x, test_data_addr[8]));

		/* test itr_find */
		iarray_itr_find(a, itr, test_data_addr[0]);
		dbg_assert(itr_equals(itr, iarray_itr_end(a)));
		iarray_itr_find(a, itr, test_data_addr[9]);
		dbg_assert(itr_equals(itr, iarray_itr_end(a)));

		for (current = 1; current <= 8; current ++) {
			iarray_itr_find(a, itr, test_data_addr[current]);
			dbg_assert(!itr_equals(itr, iarray_itr_end(a)));
		}

		/* test itr_remove */
		iarray_itr_find(a, itr, test_data_addr[4]);
		dbg_assert(!itr_equals(itr, iarray_itr_end(a)));

		iarray_itr_remove(a, itr);
		/* now the a is { 1, 2, 3,  , 5, 6, 7, 8 } */
		dbg_assert(iarray_size(a) == 7);
		for (current = 0; current <= 9; current ++) {
			iarray_itr_find(a, itr, test_data_addr[current]);
			if (current == 0 || current == 9 || current == 4) {
				dbg_assert(itr_equals(itr, iarray_itr_end(a)));
			}
			else {
				dbg_assert(!itr_equals(itr, iarray_itr_end(a)));
			}
		}

		/* test continuous itr_remove */
		iarray_itr_find(a, itr, test_data_addr[5]);
		dbg_assert(!itr_equals(itr, iarray_itr_end(a)));

		iarray_itr_remove(a, itr);
		/* now the a is { 1, 2, 3,  , 6, 7, 8 } */
		dbg_assert(iarray_size(a) == 6);
		for (current = 0; current <= 9; current ++) {
			iarray_itr_find(a, itr, test_data_addr[current]);
			if (current == 0 || current == 9 || current == 4 || current == 5) {
				dbg_assert(itr_equals(itr, iarray_itr_end(a)));
			}
			else {
				dbg_assert(!itr_equals(itr, iarray_itr_end(a)));
			}
		}

		/* test insert_before */
		iarray_itr_find(a, itr, test_data_addr[6]);
		dbg_assert(!itr_equals(itr, iarray_itr_end(a)));

		/* now the a is { 1, 2, 3,  6, 7, 8 } */
		for (current = 1, iarray_itr_assign(a, itr, itr_begin); 
				!itr_equals(itr, end); 
				current ++) {
			if (current != 4 && current != 5) {
				x = itr_get_ref(itr);
				dbg_assert(td_traits->__equals(x, test_data_addr[current]));
				itr_to_next(itr);
			}
		}

		iarray_itr_find(a, itr, test_data_addr[3]);
		dbg_assert(!itr_equals(itr, iarray_itr_end(a)));

		/* now the a is { 1, 2, 3,  6, 7, 8 } */
		iarray_itr_assign(a, itr, itr_begin);
		for (current = 1; current <= 8; current ++) {
			if (current == 4 || current == 5) 
				continue;
			x = itr_get_ref(itr);
			dbg_assert(td_traits->__equals(x, test_data_addr[current]));
			itr_to_next(itr);
		}

		itr_destroy(itr);
	}

	return;
}

/* TODO: iarray_at is not tested yet */
static void array_test_basic_operation(_object* a, struct test_data_desc* td_desc) {
	address *test_data_addr   = td_desc->data_repo;
	unknown_traits *td_traits = td_desc->data_traits;
	iarray_clear(a);
	dbg_assert(iarray_empty(a));

	iarray_add_front(a, test_data_addr[4]);
	iarray_add_back (a, test_data_addr[5]);
	iarray_add_front(a, test_data_addr[3]);
	iarray_add_back (a, test_data_addr[6]);
	iarray_add_front(a, test_data_addr[2]);
	iarray_add_back (a, test_data_addr[7]);
	iarray_add_front(a, test_data_addr[1]);
	iarray_add_back (a, test_data_addr[8]);
	/* now the a contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	dbg_assert(iarray_size(a) == 8);
	{
		intptr_t current = 1;

		dbg_assert(td_traits->__equals(iarray_front(a), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iarray_back (a), test_data_addr[8]));

		for (; current <= 8; current ++) {
			dbg_assert(iarray_contains(a, test_data_addr[current]));
		}
		dbg_assert(iarray_contains(a, test_data_addr[0]) == false);
		dbg_assert(iarray_contains(a, test_data_addr[0]) == false);
	}

	{
		int idx = 0;
		for (idx = 0; idx < 8; idx ++) {
			dbg_assert(td_traits->__equals(iarray_at(a, idx), test_data_addr[idx + 1]));
		}
		for (idx = -1; idx >= -8; idx --) {
			dbg_assert(td_traits->__equals(iarray_at(a, idx), test_data_addr[idx + 9]));
		}
		for (idx = 8; idx < 16; idx ++) {
			dbg_assert(td_traits->__equals(iarray_at(a, idx), test_data_addr[idx - 8 + 1]));
		}
		for (idx = -9; idx >= -16; idx --) {
			dbg_assert(td_traits->__equals(iarray_at(a, idx), test_data_addr[idx + 17]));
		}
	}

	/* test remove, delete the even ones */
	dbg_assert(iarray_size(a) == 8);
	{
		/* now the a contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t x;
		bool res;

		for (x = 2; x <= 8; x +=2) {
			res = iarray_remove(a, test_data_addr[x]);
			dbg_assert(res == 1);
		}

		dbg_assert(iarray_size(a) == 4);
		for (x = 1; x <= 8; x ++) {
			bool exist = x % 2 ? true : false;
			dbg_assert(iarray_contains(a, test_data_addr[x]) == exist);
		}

		dbg_assert(iarray_contains(a, test_data_addr[0]) == false);
		dbg_assert(iarray_contains(a, test_data_addr[9]) == false);
	}

	/* test remove_front and remove_back */
	dbg_assert(iarray_size(a) == 4);
	{
		/* now the a is {1, 3, 5, 7} */

		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[1]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[7]));

		iarray_remove_front(a);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[3]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[7]));
		dbg_assert(iarray_size(a) == 3);
		dbg_assert(iarray_empty(a) == false);

		iarray_remove_back(a);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[3]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[5]));
		dbg_assert(iarray_size(a) == 2);
		dbg_assert(iarray_empty(a) == false);

		iarray_remove_front(a);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[5]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[5]));
		dbg_assert(iarray_size(a) == 1);
		dbg_assert(iarray_empty(a) == false);

		iarray_remove_back(a);
		dbg_assert(iarray_front(a) == NULL);
		dbg_assert(iarray_back (a) == NULL);
		dbg_assert(iarray_size(a) == 0);
		dbg_assert(iarray_empty(a));

		/* now the a is empty, trying to remove from an empty a */
		iarray_remove_front(a);
		dbg_assert(iarray_size(a) == 0);
		dbg_assert(iarray_empty(a) == true);
		dbg_assert(iarray_front(a) == NULL);
		dbg_assert(iarray_back (a) == NULL);

		iarray_remove_back(a);
		dbg_assert(iarray_size(a) == 0);
		dbg_assert(iarray_empty(a) == true);
		dbg_assert(iarray_front(a) == NULL);
		dbg_assert(iarray_back (a) == NULL);

		/* trying to add element into the a after removing from empty a */
		iarray_add_front(a, test_data_addr[2]);
		/* now the a is { 2 } */
		dbg_assert(iarray_size(a) == 1);
		dbg_assert(iarray_empty(a) == false);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[2]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[2]));

		iarray_add_back (a, test_data_addr[3]);
		/* now the a is { 2, 3 } */
		dbg_assert(iarray_size(a) == 2);
		dbg_assert(iarray_empty(a) == false);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[2]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[3]));

		iarray_add_front(a, test_data_addr[1]);
		/* now the a is { 1, 2, 3 } */
		dbg_assert(iarray_size(a) == 3);
		dbg_assert(iarray_empty(a) == false);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[1]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[3]));

		iarray_add_back (a, test_data_addr[4]);
		/* now the a is { 1, 2, 3, 4 } */
		dbg_assert(iarray_size(a) == 4);
		dbg_assert(iarray_empty(a) == false);
		dbg_assert(td_traits->__equals(iarray_front(a),test_data_addr[1]));
		dbg_assert(td_traits->__equals(iarray_back (a),test_data_addr[4]));

		iarray_clear(a);
	}

	/* test clear */
	{
		/* now the a is empty */
		dbg_assert(iarray_empty(a));

		dbg_assert(iarray_front(a) == NULL);
		dbg_assert(iarray_back (a) == NULL);
		
		/* clear an empty a */
		iarray_clear(a);
		dbg_assert(iarray_empty(a));

		iarray_add_back(a, test_data_addr[1]);
		iarray_add_back(a, test_data_addr[1]);
		iarray_add_back(a, test_data_addr[1]);

		/* now the a is { 1, 1, 1 } */
		iarray_clear(a);
		dbg_assert(iarray_empty(a));
	}
}

void array_test_basic(_object* a, struct test_data_desc* td_desc) {
	array_test_basic_operation(a, td_desc);
	array_test_basic_itr_operation(a, td_desc);
}

void array_test_memory(_object* a) {
}

void array_test_bench(_object* a) {
	/* should this exist, or it just compares with the raw array_link to show the overhead of 'virtual' functions */
}

