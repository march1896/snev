#include <cntr2/iobject.h>
#include <cntr2/iset.h>
#include <cntr2/ifactory.h>

#include "ele.data.h"
#include "test_util.h"
#include "iset.test.h"
#include "imset.test.h"

static void osplay_as_set_basic_test() {
	_object* a = cntr_create_osplay(&int_traits);
	test_run_single_v("int", (pf_test_case_v)set_test_basic, a, &int_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&longint_traits);
	test_run_single_v("longint", (pf_test_case_v)set_test_basic, a, &longint_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&float_traits);
	test_run_single_v("float", (pf_test_case_v)set_test_basic, a, &float_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&double_traits);
	test_run_single_v("double", (pf_test_case_v)set_test_basic, a, &double_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&char_traits);
	test_run_single_v("char", (pf_test_case_v)set_test_basic, a, &char_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&string_traits);
	test_run_single_v("string", (pf_test_case_v)set_test_basic, a, &string_test_desc);
	iobject_destroy(a);
}

static void osplay_as_mset_basic_test() {
	_object* a = cntr_create_osplay(&int_traits);
	test_run_single_v("int", (pf_test_case_v)mset_test_basic, a, &int_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&longint_traits);
	test_run_single_v("longint", (pf_test_case_v)mset_test_basic, a, &longint_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&float_traits);
	test_run_single_v("float", (pf_test_case_v)mset_test_basic, a, &float_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&double_traits);
	test_run_single_v("double", (pf_test_case_v)mset_test_basic, a, &double_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&char_traits);
	test_run_single_v("char", (pf_test_case_v)mset_test_basic, a, &char_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&string_traits);
	test_run_single_v("string", (pf_test_case_v)mset_test_basic, a, &string_test_desc);
	iobject_destroy(a);
}

static void osplay_as_set_bench_test() {
	_object* a = cntr_create_osplay_a(&int_traits, NULL);
	test_run_bench_v("int", (pf_test_case_v)set_test_bench, a, &int_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay_a(&double_traits, NULL);
	test_run_bench_v("double", (pf_test_case_v)set_test_bench, a, &double_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&string_traits);
	test_run_bench_v("string", (pf_test_case_v)set_test_bench, a, &string_test_desc);
	iobject_destroy(a);
}

static void osplay_as_mset_bench_test() {
	_object* a = cntr_create_osplay_a(&int_traits, NULL);
	test_run_bench_v("int", (pf_test_case_v)mset_test_bench, a, &int_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay_a(&double_traits, NULL);
	test_run_bench_v("double", (pf_test_case_v)mset_test_bench, a, &double_test_desc);
	iobject_destroy(a);

	a = cntr_create_osplay(&string_traits);
	test_run_bench_v("string", (pf_test_case_v)mset_test_bench, a, &string_test_desc);
	iobject_destroy(a);
}

static void osplay_correctness_test() {
	test_run_single("osplay as set basic test", osplay_as_set_basic_test);
	test_run_single("osplay as mset basic test", osplay_as_mset_basic_test);
}

static void osplay_performance_test() {
	test_run_single("osplay as set bench test", osplay_as_set_bench_test);
	test_run_single("osplay as mset bench test", osplay_as_mset_bench_test);
}

void cntr2_osplay_test() {
	test_run_single("cntr_splay correctness", osplay_correctness_test);

	test_run_bench("cntr_splay performance", osplay_performance_test);
}
