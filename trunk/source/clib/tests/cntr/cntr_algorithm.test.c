#include <cntr.h>

#include <cntr/cntr_test_util.h>

#include <stdio.h>
#include <string.h>

/*
static void add_find_remove_performance_test(TC_TYPE ct, TD_TYPE dt, TD_LENGTH dl) {
	cntr c;
	citer_dos(first, NULL);
	citer_dos(second, NULL);
	int rep, i, length, ulength;
	clock_t start_c, end_c;
	float find_used;

	if (dt == TD_UNIQUE) {
		if (ct >= TC_BST && ct <= TC_234T) 
			return;
	}

	printf("\n%s add/find/remove performance test with %s data\n", cntr_name(ct), data_order_name(dt));

	c = cntr_create(ct);

	generate_test_data(dt, dl, &length, &ulength);

	printf("add: ");
	start_c = clock();	
	for (i = 0; i < length; i ++) cntr_add(c, (void*)rawdata[i]);
	end_c = clock();
	printf("used %f\n", (float)(end_c - start_c)/CLOCKS_PER_SEC);

	printf("find: ");
	start_c = clock();
	for (rep = 0; rep < 20; rep ++) {
		for (i = 0; i < length; i ++) {
			bool find = cntr_find(c, (void*)rawdata[i], first);
			dbg_assert(find);
		}
	}
	end_c = clock();
	printf("used %f\n", (float)(end_c - start_c)/CLOCKS_PER_SEC);
	find_used = (float)(end_c - start_c)/CLOCKS_PER_SEC;


	printf("remove: ");
	find_used = 0.0;
	start_c = clock();
	for (i = 0; i < ulength; i ++) {
		bool find = cntr_find(c, (void*)uniquedata[i], first);

		cntr_remove(c, first, first);
	}
	end_c = clock();
	printf("used %f\n", (float)(end_c - start_c)/CLOCKS_PER_SEC - find_used);

	cntr_clear(c);
	dbg_assert(cntr_size(c) == 0);

	cntr_destroy(c);
}
*/

static void reverse_test(TC_TYPE ct, TD_TYPE dt, TD_LENGTH dl) {
	cntr c, d;
	citer_dos(first, NULL);
	citer_dos(second, NULL);

	citer_dos(aug, NULL);
	int i, length, ulength;

	printf("%s reverse function test with %s data\n", cntr_name(ct), data_order_name(dt));

	c = cntr_create(ct);
	d = cntr_create(ct);

	generate_test_data(dt, dl, &length, &ulength);

	for (i = 0; i < length; i ++) {
		cntr_add(c, (void*)rawdata[i]);
		cntr_add(d, (void*)rawdata[i]);
	}

	cntr_citer_begin(c, first);
	cntr_citer_end(c, second);

	citer_reverse(first, second);

	cntr_citer_begin(c, aug);
	dbg_assert(citer_equal(first, aug));
	cntr_citer_end(c, aug);
	dbg_assert(citer_equal(second, aug));

	cntr_citer_end(d, aug);
	for (; !citer_equal(first, second); citer_to_next(first), citer_to_prev(aug)) {
		dbg_assert(citer_get_ref(first) == citer_get_ref(aug));
	}

	cntr_destroy(c);
	cntr_destroy(d);
}

static void print_element(citer itr) {
	printf("%d ", (int)citer_get_ref(itr));
}
#define PERMUTATION_LENGTH 5
static void permutation_test(TC_TYPE ct) {
	cntr c;
	citer_dos(first, NULL);
	citer_dos(last, NULL);

	int i, count, ans;

	printf("%s next permutation test\n", cntr_name(ct));

	c = cntr_create(ct);

	ans = 1;
	for (i = 0; i < PERMUTATION_LENGTH; i ++) {
		cntr_add(c, (void*)i);

		ans *= (i + 1);
	}

	cntr_citer_begin(c, first);
	cntr_citer_end(c, last);
	count = 0;
	do {
		citer_for_each(first, last, print_element);
		printf("\n");
		count ++;
	} while (citer_next_permutation(first, last, cntr_int_compare));
	
	dbg_assert(count == ans);

	do {
		citer_for_each(first, last, print_element);
		printf("\n");
		count --;
	} while (citer_prev_permutation(first, last, cntr_int_compare));

	dbg_assert(count == 0);

	cntr_destroy(c);
}

static void for_each_test(TC_TYPE ct) {
	cntr c;
	citer_dos(first, NULL);
	citer_dos(last, NULL);

	printf("%s for_each test\n", cntr_name(ct));

	c = cntr_create(ct);
	// for an empty container, the begin, end iterator is not well defined,
	// so the for_each function will not handle the situation well.
	cntr_citer_begin(c, first);
	cntr_citer_end(c, last);

	// will we crash here ?
	// [BUG, BUG]
	//citer_for_each(first, last, print_element);

	cntr_destroy(c);

	return;
}

static void reverse_correctness_test() {
	int i, j;
	printf("reverse correctness test start\n");
	for (i = TC_LIST; i <= TC_ARRAY; i ++) {
		for (j = TD_INCREASE; j < TD_END; j ++) {
			reverse_test((TC_TYPE)i, (TD_TYPE)j, TL_CORRECTNESS);
		}
	}
	printf("reverse correctness test end\n");
}

static void prev_next_permutation_test() {
	int i;
	printf("permutation correctness test start\n");
	for (i = TC_LIST; i <= TC_ARRAY; i ++) {
		permutation_test((TC_TYPE)i);
	}
	printf("permutation correctness test end\n");
}

static void for_each_tests() {
	int i;
	printf("for each correctness test start\n");
	for (i = TC_LIST; i <= TC_ARRAY; i ++) {
		for_each_test((TC_TYPE)i);
	}
	printf("for each correctness test end\n");
}

void algorithm_base_test() {
	do_test("reverse correctness", reverse_correctness_test);

	do_test("permutation correctness", prev_next_permutation_test);

	do_test("for each correctness", for_each_tests);
}

