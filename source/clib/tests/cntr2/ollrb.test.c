#include "cntr2/iset.h"
#include "cntr2/ifactory.h"

#include "test_util.h"
#include "cntr2/iset.test.h"

static int int_compare(const void* a, const void* b) {
	if ((intptr_t)a < (intptr_t)b) {
		return -1;
	}
	else if ((intptr_t)a > (intptr_t)b) {
		return 1;
	}
	return 0;
}

static void __correctness_test() {
	{
		iset set = as_set(cntr_create_ollrb(int_compare));
		set_test_basic(set);
		iset_destroy(set);
	}
}

static void __performance_test() {
	log_printline("add find remove performance test start");
	log_printline("add find remove performance test end");
}

void cntr2_ollrb_test() {
	test_run_single("cntr cntr_base correctness", __correctness_test);

	//test_run_single("cntr cntr_base performance", __performance_test);
}
