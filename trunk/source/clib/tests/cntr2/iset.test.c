#include "cntr2/iset.h"
#include "cntr2/iitr.h"

#include "iset.test.h"
#include "test_util.h"

static void set_test_basic_itr_operation(iobject* set) {
	iset_clear(set);
	dbg_assert(iset_empty(set));

	iset_insert(set, (void*)(intptr_t)4);
	iset_insert(set, (void*)(intptr_t)5);
	iset_insert(set, (void*)(intptr_t)3);
	iset_insert(set, (void*)(intptr_t)6);
	iset_insert(set, (void*)(intptr_t)2);
	iset_insert(set, (void*)(intptr_t)7);
	iset_insert(set, (void*)(intptr_t)1);
	iset_insert(set, (void*)(intptr_t)8);
	/* now the set contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	dbg_assert(iset_size(set) == 8);
	{
		iterator itr = iset_itr_create(set, itr_begin);
		const_iterator end = iset_itr_end(set);
		intptr_t current = 1;

		/* traverse the set */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)current);
			current ++;
		}

		/* test itr_assign */
		iset_itr_assign(set, itr, itr_begin);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)1);
		iset_itr_assign(set, itr, itr_end);
		itr_to_prev(itr);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)8);

		/* test itr_find */
		iset_itr_find(set, itr, (void*)(intptr_t)0);
		dbg_assert(itr_equals(itr, iset_itr_end(set)));
		iset_itr_find(set, itr, (void*)(intptr_t)9);
		dbg_assert(itr_equals(itr, iset_itr_end(set)));

		for (current = 1; current <= 8; current ++) {
			iset_itr_find(set, itr, (void*)current);
			dbg_assert(!itr_equals(itr, iset_itr_end(set)));
		}

		/* test itr_remove */
		iset_itr_find(set, itr, (void*)(intptr_t)4);
		dbg_assert(!itr_equals(itr, iset_itr_end(set)));

		current = (intptr_t)iset_itr_remove(set, itr);
		/* now the set is { 1, 2, 3,  , 5, 6, 7, 8 } */
		dbg_assert(current == 4);
		dbg_assert(iset_size(set) == 7);
		for (current = 0; current <= 9; current ++) {
			iset_itr_find(set, itr, (void*)current);
			if (current == 0 || current == 9 || current == 4) {
				dbg_assert(itr_equals(itr, iset_itr_end(set)));
			}
			else {
				dbg_assert(!itr_equals(itr, iset_itr_end(set)));
			}
		}

		/* test continous itr_remove */
		iset_itr_find(set, itr, (void*)(intptr_t)5);
		dbg_assert(!itr_equals(itr, iset_itr_end(set)));

		current = (intptr_t)iset_itr_remove(set, itr);
		/* now the set is { 1, 2, 3,   6, 7, 8 } */
		dbg_assert(current == 5);
		dbg_assert(iset_size(set) == 6);
		for (current = 0; current <= 9; current ++) {
			iset_itr_find(set, itr, (void*)current);
			if (current == 0 || current == 9 || current == 4 || current == 5) {
				dbg_assert(itr_equals(itr, iset_itr_end(set)));
			}
			else {
				dbg_assert(!itr_equals(itr, iset_itr_end(set)));
			}
		}

		itr_destroy(itr);
	}

	return;
}

static void set_test_basic_operation(iobject* set) {
	iset_clear(set);
	dbg_assert(iset_empty(set));

	{
		bool res;
		res = iset_insert(set, (void*)(intptr_t)4);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)5);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)3);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)6);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)2);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)7);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)1);
		dbg_assert(res == true);
		res = iset_insert(set, (void*)(intptr_t)8);
		dbg_assert(res == true);
	}
	/* now the set contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
	dbg_assert(iset_size(set) == 8);
	{
		intptr_t current = 1;

		for (; current <= 8; current ++) {
			dbg_assert(iset_contains(set, (void*)current));
		}
		dbg_assert(iset_contains(set, (void*)(intptr_t)0) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)9) == false);
	}

	/* test remove, delete the even ones */
	dbg_assert(iset_size(set) == 8);
	{
		/* now the set contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t x;
		bool res;

		for (x = 2; x <= 8; x +=2) {
			res = iset_remove(set, (void*)x);
			dbg_assert(res == 1);
		}

		dbg_assert(iset_size(set) == 4);
		for (x = 1; x <= 8; x ++) {
			bool exist = x % 2 ? true : false;
			dbg_assert(iset_contains(set, (void*)x) == exist);
		}

		dbg_assert(iset_contains(set, (void*)(intptr_t)0) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)9) == false);
	}

	dbg_assert(iset_size(set) == 4);
	{
		bool res;

		/* remove elements one by one */
		res = (intptr_t)iset_remove(set, (void*)(intptr_t)3);
		/* now the set is { 1,  5, 7 } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 3);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)3) == false);

		/* try to remove another 3, should failed */
		res = (intptr_t)iset_remove(set, (void*)(intptr_t)3);
		/* now the set is { 1,  5, 7 } */
		dbg_assert(res == false);
		dbg_assert(iset_size(set) == 3);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)3) == false);

		res = (intptr_t)iset_remove(set, (void*)(intptr_t)5);
		/* now the set is { 1,   7 } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 2);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)5) == false);
		
		res = (intptr_t)iset_remove(set, (void*)(intptr_t)1);
		/* now the set is { 7 } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 1);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)1) == false);

		res = (intptr_t)iset_remove(set, (void*)(intptr_t)7);
		/* now the set is { } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 0);
		dbg_assert(iset_empty(set) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)7) == false);

		/* try to remove element from empty set */
		res = (intptr_t)iset_remove(set, (void*)(intptr_t)7);
		/* now the set is { } */
		dbg_assert(res == false);
		dbg_assert(iset_size(set) == 0);
		dbg_assert(iset_empty(set) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)7) == false);

		/* trying to add element into the set after removing from empty set */
		res = iset_insert(set, (void*)(intptr_t)2);
		/* now the set is { 2 } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 1);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)2) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)0) == false);

		res = iset_insert(set, (void*)(intptr_t)3);
		/* now the set is { 2, 3 } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 2);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)2) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)3) == true);

		iset_clear(set);
	}

	dbg_assert(iset_empty(set) == true);
	{
		bool res;
		/* try to add same element to the set */
		res = iset_insert(set, (void*)(intptr_t)1);
		/* now the set is { 1 } */
		dbg_assert(res == true);
		dbg_assert(iset_size(set) == 1);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)1) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)0) == false);

		/* insert 1 again */
		res = iset_insert(set, (void*)(intptr_t)1);
		/* now the set is { 1 } */
		dbg_assert(res == false);
		dbg_assert(iset_size(set) == 1);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)1) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)0) == false);

		/* insert 1 again and again */
		res = iset_insert(set, (void*)(intptr_t)1);
		/* now the set is { 1 } */
		dbg_assert(res == false);
		dbg_assert(iset_size(set) == 1);
		dbg_assert(iset_empty(set) == false);
		dbg_assert(iset_contains(set, (void*)(intptr_t)1) == true);
		dbg_assert(iset_contains(set, (void*)(intptr_t)0) == false);
	}

	/* test clear */
	{
		iset_clear(set);
		/* now the set is empty */
		dbg_assert(iset_empty(set));
		
		/* clear an empty set */
		iset_clear(set);
		dbg_assert(iset_empty(set));

		iset_insert(set, (void*)(intptr_t)1);
		iset_insert(set, (void*)(intptr_t)1);
		iset_insert(set, (void*)(intptr_t)1);

		/* now the set is { 1, 1, 1 } */
		iset_clear(set);
		dbg_assert(iset_empty(set));
	}
}

void set_test_dataset(iobject* set, int dataset_size) {
	intptr_t x = 0;
	bool res = false;

	iset_clear(set);
	dbg_assert(iset_empty(set));

	for (x = 0; x < (intptr_t)dataset_size; x ++) {
		res = iset_insert(set, (void*)x);
		dbg_assert(res == true);
	}
	dbg_assert(iset_size(set) == dataset_size);

	/* insert the data set again */
	for (x = 0; x < (intptr_t)dataset_size; x ++) {
		res = iset_insert(set, (void*)x);
		dbg_assert(res == false);
	}
	dbg_assert(iset_size(set) == dataset_size);

	for (x = 0; x < (intptr_t)dataset_size; x ++) {
		res = iset_contains(set, (void*)x);
		dbg_assert(res == true);
	}
	dbg_assert(iset_contains(set, (void*)(x + 1)) == false);

	for (x = 0; x < (intptr_t)dataset_size; x ++) {
		res = iset_remove(set, (void*)x);
		dbg_assert(res == true);
	}
	dbg_assert(iset_size(set) == 0);
}

void set_test_random_operation(iobject* mset, int max_diff_types, int dataset_size) {
	int i;

	iset_clear(mset);
	dbg_assert(iset_empty(mset));

	for (i = 0; i < dataset_size; i ++) {
		intptr_t x = rand() % max_diff_types;
		bool found = iset_contains(mset, (void*)x);

		if (!found) {
			iset_insert(mset, (void*)x);
		}
		else {
			bool res = iset_remove(mset, (void*)x);
			dbg_assert(res == true);
		}
	}

	iset_clear(mset);
}

void set_test_basic(iobject* set) {
	set_test_basic_operation(set);
	set_test_basic_itr_operation(set);
}

void set_test_memory(iobject* set) {
}

void set_test_bench(iobject* set, int max_diff_type, int dataset_size) {
	set_test_dataset(set, max_diff_type); 
	set_test_random_operation(set, max_diff_type, dataset_size);
}
