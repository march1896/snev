#include <stdio.h>
#include <string.h>

#include <cntr2/iobject.h>
#include <cntr2/ilist.h>
#include <cntr2/iqueue.h>
#include <cntr2/istack.h>
#include <cntr2/ifactory.h>

#include "ele.data.h"
#include "test_util.h"
#include "ilist.test.h"
#include "iqueue.test.h"
#include "istack.test.h"

static void olist_correctness_test() {
	_object* a = NULL;
	{
		log_inc_tab(true);
		log_printline("[as list]");
		
		a = cntr_create_olist(&int_traits);
		test_run_single_v("int", (pf_test_case_v)list_test_basic, a, &int_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&longint_traits);
		test_run_single_v("longint", (pf_test_case_v)list_test_basic, a, &longint_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&float_traits);
		test_run_single_v("float", (pf_test_case_v)list_test_basic, a, &float_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&double_traits);
		test_run_single_v("double", (pf_test_case_v)list_test_basic, a, &double_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&char_traits);
		test_run_single_v("char", (pf_test_case_v)list_test_basic, a, &char_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&string_traits);
		test_run_single_v("string", (pf_test_case_v)list_test_basic, a, &string_test_desc);
		iobject_destroy(a);
		log_inc_tab(false);
	}
	
	{
		log_inc_tab(true);
		log_printline("[as queue]");
		a = cntr_create_olist(&int_traits);
		test_run_single_v("int", (pf_test_case_v)queue_test_basic, a, &int_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&double_traits);
		test_run_single_v("double", (pf_test_case_v)queue_test_basic, a, &double_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&char_traits);
		test_run_single_v("char", (pf_test_case_v)queue_test_basic, a, &char_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&string_traits);
		test_run_single_v("string", (pf_test_case_v)queue_test_basic, a, &string_test_desc);
		iobject_destroy(a);
		log_inc_tab(false);
	}	

	{
		log_inc_tab(true);
		log_printline("[as stack]");
		a = cntr_create_olist(&int_traits);
		test_run_single_v("int", (pf_test_case_v)stack_test_basic, a, &int_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&double_traits);
		test_run_single_v("double", (pf_test_case_v)stack_test_basic, a, &double_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&char_traits);
		test_run_single_v("char", (pf_test_case_v)stack_test_basic, a, &char_test_desc);
		iobject_destroy(a);

		a = cntr_create_olist(&string_traits);
		test_run_single_v("string", (pf_test_case_v)stack_test_basic, a, &string_test_desc);
		iobject_destroy(a);
		log_inc_tab(false);
	}	
}

static void olist_performance_test() {
	/*
	log_printline("add find remove performance test start");
	log_printline("add find remove performance test end");
	*/
}

void cntr2_odlist_test() {
	test_run_single("cntr cntr_base correctness", olist_correctness_test);

	test_run_bench("cntr cntr_base performance", olist_performance_test);
}
