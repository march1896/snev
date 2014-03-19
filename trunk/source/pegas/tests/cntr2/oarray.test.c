#include <stdio.h>
#include <string.h>

#include <cntr2/iobject.h>
#include <cntr2/iarray.h>
#include <cntr2/iqueue.h>
#include <cntr2/istack.h>
#include <cntr2/ifactory.h>

#include "ele.data.h"
#include "test_util.h"
#include "iarray.test.h"
#include "iqueue.test.h"
#include "istack.test.h"

static void oarray_correctness_test() {
	_object* a = NULL;
	{
		log_inc_tab(true);
		log_printline("[as array]");
		a = cntr_create_oarray(&int_traits);
		//array_test_basic(a, &int_test_desc);
		test_run_single_v("int", (pf_test_case_v)array_test_basic, a, &int_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&longint_traits);
		test_run_single_v("longint", (pf_test_case_v)array_test_basic, a, &longint_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&float_traits);
		test_run_single_v("float", (pf_test_case_v)array_test_basic, a, &float_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&double_traits);
		test_run_single_v("double", (pf_test_case_v)array_test_basic, a, &double_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&char_traits);
		test_run_single_v("char", (pf_test_case_v)array_test_basic, a, &char_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&string_traits);
		test_run_single_v("string", (pf_test_case_v)array_test_basic, a, &string_test_desc);
		iobject_destroy(a);
		log_inc_tab(false);
	}
	
	{
		log_inc_tab(true);
		log_printline("[as queue]");
		a = cntr_create_oarray(&int_traits);
		test_run_single_v("int", (pf_test_case_v)queue_test_basic, a, &int_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&double_traits);
		test_run_single_v("double", (pf_test_case_v)queue_test_basic, a, &double_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&char_traits);
		test_run_single_v("char", (pf_test_case_v)queue_test_basic, a, &char_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&string_traits);
		test_run_single_v("string", (pf_test_case_v)queue_test_basic, a, &string_test_desc);
		iobject_destroy(a);
		log_inc_tab(false);
	}	

	{		
		log_inc_tab(true);
		log_printline("[as stack]");
		a = cntr_create_oarray(&int_traits);
		test_run_single_v("int", (pf_test_case_v)stack_test_basic, a, &int_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&double_traits);
		test_run_single_v("double", (pf_test_case_v)stack_test_basic, a, &double_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&char_traits);
		test_run_single_v("char", (pf_test_case_v)stack_test_basic, a, &char_test_desc);
		iobject_destroy(a);

		a = cntr_create_oarray(&string_traits);
		test_run_single_v("string", (pf_test_case_v)stack_test_basic, a, &string_test_desc);
		iobject_destroy(a);
		log_inc_tab(false);
	}	
}

static void oarray_performance_test() {
	/*
	log_printline("add find remove performance test start");
	log_printline("add find remove performance test end");
	*/
}

void cntr2_oarray_test() {
	test_run_single("cntr cntr_base correctness", oarray_correctness_test);

	test_run_bench("cntr cntr_base performance", oarray_performance_test);
}
