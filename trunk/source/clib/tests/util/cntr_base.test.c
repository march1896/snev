#include <cntr.h>
#include <test_util.h>
#include <stdio.h>
#include <string.h>

typedef cntr (*pf_cntr_create)();
static void add_find_remove_correctness_test(
	const char* cntr_name,
	const char* data_order_name,
	pf_cntr_create cntr_create,
	int data_order,
	int data_length
	) {
	cntr c;
	citer_dos(first, NULL);
	citer_dos(second, NULL);
	int i, ul;

	printf("%s add/find/remove test with %s data\n", cntr_name, data_order_name);

	if (strcmp(cntr_name, "bst") == 0) {
		//c = cntr_create(cntr_int_compare);
	}
	else {
		c = cntr_create();
	}
	ul = generate_data(data_order, data_length);

	for (i = 0; i < data_length; i ++) cntr_add(c, (void*)x[i]);

	/* test find */
	for (i = 0; i < data_length; i ++) {
		bool find = cntr_find(c, (void*)x[i], first);
		dbg_assert(find);
	}

	for (i = data_length + 1; i < data_length + 50; i ++) {
		bool find = cntr_find(c, (void*)i, first);
		dbg_assert(find == false);
	}

	/* test remove, remove first half elements */
	for (i = 0; i < data_length / 2; i ++) {
		bool find = cntr_find(c, (void*)x[i], first);
		cntr_remove(c, first, first);
		dbg_assert(cntr_size(c) == data_length - i - 1);
	}

	cntr_clear(c);
	dbg_assert(cntr_size(c) == 0);

	cntr_destroy(c);
}

typedef cntr (*pf_set_create)(pf_compare_object);
static void set_add_find_remove_correctness_test(
	const char* cntr_name,
	const char* data_order_name,
	pf_set_create cntr_create,
	int data_order,
	int data_length
	) {
		cntr c;
		citer_dos(first, NULL);
		citer_dos(second, NULL);
		int i, ul;

		printf("%s add/find/remove test with %s data\n", cntr_name, data_order_name);

		c = cntr_create(cntr_int_compare);

		ul = generate_data(data_order, data_length);

		for (i = 0; i < data_length; i ++) cntr_add(c, (void*)x[i]);

		/* test find */
		for (i = 0; i < data_length; i ++) {
			bool find = cntr_find(c, (void*)x[i], first);
			dbg_assert(find);
		}

		for (i = data_length + 1; i < data_length + 50; i ++) {
			bool find = cntr_find(c, (void*)i, first);
			dbg_assert(find == false);
		}

		/* test remove, remove first half elements */
		for (i = 0; i < ul / 2; i ++) {
			bool find = cntr_find(c, (void*)y[i], first);
			cntr_remove(c, first, first);
			dbg_assert(cntr_size(c) == ul - i - 1);
		}

		cntr_clear(c);
		dbg_assert(cntr_size(c) == 0);

		cntr_destroy(c);
}

static void cntr_base_memory_test() {
	
}


const char* data_order[] = {"increase", "decrease", "random" };
const char* linear_name[] = {"list", "array"};
pf_cntr_create linear_create[] = { cntr_create_as_list, cntr_create_as_array};
const char* set_name[] = { "bst" };
pf_set_create set_create[] = { cntr_create_as_bst };

static void cntr_base_correctness_test() {
	int i, j;
	printf("add find remove test start\n");
	for (i = 0; i < 2; i ++) {
		for (j = 0; j < 3; j ++) {
			add_find_remove_correctness_test(linear_name[i], data_order[j], linear_create[i], j, clength);
		}
	}
	for (i = 0; i < 1; i ++) {
		for (j = 0; j < 3; j ++) {
			set_add_find_remove_correctness_test(set_name[i], data_order[j], set_create[i], j, clength);
		}
	}
	printf("add find remove test end\n\n");
}

static void cntr_base_performance_test() {

}

void cntr_base_test() {
	do_test("cntr cntr_base correctness", cntr_base_correctness_test);

	do_test("cntr cntr_base performance", cntr_base_performance_test);
}
