#include <cntr2/iset.h>
#include <cntr2/ifactory.h>
#include <cntr2/oallocator.h>

#include "test_util.h"
#include "cntr2/iset.test.h"
#include "cntr2/imset.test.h"

static int int_compare(const void* a, const void* b) {
	if ((intptr_t)a < (intptr_t)b) {
		return -1;
	}
	else if ((intptr_t)a > (intptr_t)b) {
		return 1;
	}
	return 0;
}

static void __as_set_basic_test() {
	iset set = as_set(cntr_create_oskiplist(int_compare));
	set_test_basic(set);
	iset_destroy(set);
}

static void __as_mset_basic_test() {
	imset mset = as_mset(cntr_create_oskiplist(int_compare));
	mset_test_basic(mset);
	imset_destroy(mset);
}

static void __as_set_bench_test() {
	iset set = as_set(cntr_create_oskiplist_v(int_compare, NULL, NULL));
	set_test_bench(set);
	iset_destroy(set);
}

static void __as_mset_bench_test() {
	imset mset = as_mset(cntr_create_oskiplist_v(int_compare, NULL, NULL));
	mset_test_bench(mset);
	imset_destroy(mset);
}

static void __correctness_test() {
	test_run_single("oskiplist as set basic test", __as_set_basic_test);
	test_run_single("oskiplist as mset basic test", __as_mset_basic_test);
}

static void __performance_test() {
	test_run_single("oskiplist as set bench test", __as_set_bench_test);
	test_run_single("oskiplist as mset bench test", __as_mset_bench_test);
}

void cntr2_oskiplist_test() {
	test_run_single("cntr_skiplist correctness", __correctness_test);

	test_run_bench("cntr_skiplist performance", __performance_test);
}
