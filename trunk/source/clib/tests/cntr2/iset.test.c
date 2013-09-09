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

static int __num_modify; /* number of insert and remove */
static int __num_search;

static int __data_diff_type; /* number of different values to store in the set */
static int __data_max_count; /* max count of a single value */

static iobject* __set = NULL; /* to be tested */
static intptr_t* data_count = NULL;

static void __reset_data() {
	int i = 0;

	for (i = 0; i < __data_diff_type; i ++) {
		data_count[i] = 0;
	}
}

static void __create_data() {
	if (data_count != NULL) 
		free(data_count);

	data_count = (intptr_t*)malloc(sizeof(intptr_t)*__data_diff_type);

	__reset_data();
}

static void __destroy_data() {
	free(data_count);
	data_count = NULL;
}

static void set_bench_modify_randomly() {
	int i;

	log_printline("[number of modify: %d]", __num_modify);
	dbg_assert(iset_empty(__set));

	for (i = 0; i < __num_modify; i ++) {
		intptr_t x = rand() % __data_diff_type;
		bool found = iset_contains(__set, (void*)x);
		
		if (found == false) {
			iset_insert(__set, (void*)x);
			data_count[x] ++;
		} else {
			iset_remove(__set, (void*)x);
			data_count[x] --;
		}
	}

	iset_clear(__set);
}

static void set_bench_search_randomly() {
	int i;
	dbg_assert(iset_empty(__set));

	log_printline("[number of search: %d]", __num_search);

	for (i = 0; i < __data_diff_type; i ++) {
		/* insert all data one by one */
		iset_insert(__set, (void*)(intptr_t)i);
	}
	/* now the set contains __data_diff_type * 2 of data, each elememt int [0, __data_diff_type)
	 * appears at least once */

	for (i = 0; i < __num_search; i ++) {
		intptr_t x = rand() % __data_diff_type;

		bool res = iset_contains(__set, (void*)x);
		dbg_assert(res == true);
	}

	iset_clear(__set);
}


void set_test_basic(iobject* set) {
	set_test_basic_operation(set);
	set_test_basic_itr_operation(set);
}

void set_test_memory(iobject* set) {
}

void set_test_bench(iobject* set) {
	__set = set;
	__data_diff_type = 100;
	__data_max_count = 1;
	__num_modify     = 100;
	__num_search     = 100;
	iset_clear(__set);
	log_printline("[data type: %d, single data max dup: %d]", __data_diff_type, __data_max_count);
	
	__create_data();
	test_run_single("randomly modify test", set_bench_modify_randomly);
	__destroy_data();
	test_run_single("randomly search test", set_bench_search_randomly);

	__num_modify     = 100000;
	__num_search     = 100;
	__create_data();
	test_run_single("randomly modify test", set_bench_modify_randomly);
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	test_run_single("randomly search test", set_bench_search_randomly);

	__data_diff_type = 5000;
	log_printline("[data type: %d, single data max dup: %d]", __data_diff_type, __data_max_count);
	__num_modify     = 100000;
	__num_search     = 100;
	__create_data();
	test_run_single("randomly modify test", set_bench_modify_randomly);
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	test_run_single("randomly search test", set_bench_search_randomly);

	__data_diff_type = 100000;
	log_printline("[data type: %d, single data max dup: %d]", __data_diff_type, __data_max_count);
	__num_modify     = 100000;
	__num_search     = 100;

	__create_data();
	test_run_single("randomly modify test", set_bench_modify_randomly);
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	test_run_single("randomly search test", set_bench_search_randomly);

	__data_diff_type = 100000;
	log_printline("[data type: %d, single data max dup: %d]", __data_diff_type, __data_max_count);
	__num_modify     = 1000000;
	__num_search     = 100;

	__create_data();
	test_run_single("randomly modify test", set_bench_modify_randomly);
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 1000000;
	test_run_single("randomly search test", set_bench_search_randomly);
}
