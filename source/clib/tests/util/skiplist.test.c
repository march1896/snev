#include <skiplist.h>
#include <heap_global.h>

#include "test_util.h"

static int ref_compare(const void* a, const void* b) {
	if (a < b) 
		return -1;
	else if (a == b) 
		return 0;
	else 
		return 1;
}

static void skiplist_basic_test() {
	struct skiplist* list = skiplist_create(ref_compare, __global_default_alloc, __global_default_dealloc, __global_default_heap);

	{
		skiplist_insert(list, (void*)(intptr_t)2);
		skiplist_insert(list, (void*)(intptr_t)0);
		skiplist_insert(list, (void*)(intptr_t)1);
		skiplist_insert(list, (void*)(intptr_t)3);
		/* now the list contains { 0, 1, 2, 3 } */

		dbg_assert(skiplist_contains(list, (void*)(intptr_t)0) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)1) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)3) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)-1) == false);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)4) == false);

		dbg_assert(skiplist_remove(list, (void*)(intptr_t)4) == false);
		dbg_assert(skiplist_remove(list, (void*)(intptr_t)0) == true);
		dbg_assert(skiplist_remove(list, (void*)(intptr_t)2) == true);
		skiplist_insert(list, (void*)(intptr_t)5);
		skiplist_insert(list, (void*)(intptr_t)7);
		/* now the list contains { 1, 3, 5, 7 } */

		dbg_assert(skiplist_contains(list, (void*)(intptr_t)1) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)3) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)5) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)7) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == false);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)4) == false);

		skiplist_clear(list);
		/* now the list contains nothing */
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)1) == false);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)3) == false);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)5) == false);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)7) == false);

		skiplist_insert(list, (void*)(intptr_t)2);
		skiplist_insert(list, (void*)(intptr_t)1);
		skiplist_insert(list, (void*)(intptr_t)2);
		skiplist_insert(list, (void*)(intptr_t)3);
		skiplist_insert(list, (void*)(intptr_t)4);
		skiplist_insert(list, (void*)(intptr_t)4);
		/* now the list contains { 1, 2, 2, 3, 4, 4 } */

		dbg_assert(skiplist_contains(list, (void*)(intptr_t)1) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)3) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)4) == true);

		dbg_assert(skiplist_remove(list, (void*)(intptr_t)2) == true);
		/* now is { 1, 2, 3, 4, 4 } */
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == true);

		dbg_assert(skiplist_remove(list, (void*)(intptr_t)2) == true);
		/* now is { 1, 3, 4, 4 } */
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == false);
		dbg_assert(skiplist_remove(list, (void*)(intptr_t)2) == false);

		skiplist_clear(list);
	}
	/* test insert_s */
	{
		bool dup = false;
		dbg_assert(skiplist_insert_s(list, (void*)(intptr_t)2) == true);
		dbg_assert(skiplist_insert_s(list, (void*)(intptr_t)1) == true);
		dbg_assert(skiplist_insert_s(list, (void*)(intptr_t)4) == true);
		dbg_assert(skiplist_insert_s(list, (void*)(intptr_t)3) == true);
		/* now is { 1, 2, 3, 4 } */

		dbg_assert(skiplist_contains(list, (void*)(intptr_t)1) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)3) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)4) == true);

		dbg_assert(skiplist_insert_s(list, (void*)(intptr_t)2) == false);
		/* still { 1, 2, 3, 4 } */
		dbg_assert(skiplist_remove(list, (void*)(intptr_t)2) == true);
		/* { 1, 3, 4 } */
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == false);

		dbg_assert(skiplist_insert_s(list, (void*)(intptr_t)2) == true);
		dbg_assert(skiplist_contains(list, (void*)(intptr_t)2) == true);

		skiplist_clear(list);
	}

	skiplist_destroy(list);
	return;
}

static intptr_t* x_count;
static int x_count_length;
static void __reset_data() {
	int i = 0;

	for (i = 0; i < x_count_length; i ++) {
		x_count[i] = 0;
	}
}

static void __create_data(int range) {
	x_count_length = range;
	x_count = (intptr_t*)malloc(sizeof(intptr_t) * x_count_length);

	__reset_data();
}

static void __destroy_data() {
	free(x_count);
}

static int data_max_diff_type;
static int data_max_dup;
static int num_operation;

static void skiplist_insert_remove() {
	int i = 0;
	struct skiplist* list = skiplist_create(ref_compare, __global_default_alloc, __global_default_dealloc, __global_default_heap);

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;

		if (x_count[idx] == 0) {
			dbg_assert(skiplist_contains(list, (void*)(intptr_t)idx) == false);
			skiplist_insert(list, (void*)(intptr_t)idx);
			x_count[idx] ++;
		}
		else if (x_count[idx] == data_max_dup) {
			bool res = skiplist_remove(list, (void*)(intptr_t)idx);
			x_count[idx] --;
			dbg_assert(res == true);
		}
		else {
			int add = rand() % 2;
			if (add == 1) {
				skiplist_insert(list, (void*)(intptr_t)idx);
				x_count[idx] ++;
			}
			else {
				bool res = skiplist_remove(list, (void*)(intptr_t)idx);
				x_count[idx] --;
				dbg_assert(res == true);
			}
		}
	}

	skiplist_destroy(list);
}

static void skiplist_insert_remove_s() {
	int i = 0;
	struct skiplist* list = skiplist_create(ref_compare, __global_default_alloc, __global_default_dealloc, __global_default_heap);

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;

		if (x_count[idx] == 0) {
			dbg_assert(skiplist_contains(list, (void*)(intptr_t)idx) == false);
			skiplist_insert_s(list, (void*)(intptr_t)idx);
			x_count[idx] ++;
		}
		else {
			int add = rand() % 2;
			if (add == 1) {
				bool res = skiplist_insert_s(list, (void*)(intptr_t)idx);
				dbg_assert(res == false);
			}
			else {
				bool res = skiplist_remove(list, (void*)(intptr_t)idx);
				dbg_assert(res == true);
				x_count[idx] --;
			}
		}
	}

	skiplist_destroy(list);
}

static void skiplist_correctness_test() {
	data_max_diff_type = 1000;
	data_max_dup = 10;

	num_operation = 100 * data_max_diff_type;

	skiplist_basic_test();

	__create_data(data_max_diff_type);
	test_run_single("insert   /remove correctness", skiplist_insert_remove);

	__reset_data();
	test_run_single("insert_s /remove correctness", skiplist_insert_remove_s);

	__destroy_data();
}

static void skiplist_performance_test() {
	data_max_diff_type = 10000;
	data_max_dup = 10;

	num_operation = 100 * data_max_diff_type;

	__create_data(data_max_diff_type);
	test_run_single("insert   /remove performance", skiplist_insert_remove);

	__reset_data();
	test_run_single("insert_s /remove performance", skiplist_insert_remove_s);

	__destroy_data();
}

void skiplist_test() {
	test_run_single("skiplist correctness test", skiplist_correctness_test);

	test_run_bench("skiplist performance test", skiplist_performance_test);
}

