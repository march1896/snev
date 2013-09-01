#include <stdio.h>
#include <string.h>

#include "cntr2/ilist.h"
#include "cntr2/iqueue.h"
#include "cntr2/istack.h"
#include "cntr2/ifactory.h"

#include "test_util.h"
#include "cntr2/ilist.test.h"
#include "cntr2/iqueue.test.h"
#include "cntr2/istack.test.h"

static void __correctness_test() {
	{
		ilist list = as_list(cntr_create_olist());
		list_test_basic(list);
		ilist_destroy(list);
	}
	
	{
		iqueue queue = as_queue(cntr_create_olist());
		queue_test_basic(queue);
		iqueue_destroy(queue);
	}	

	{
		istack stack = as_stack(cntr_create_olist());
		stack_test_basic(stack);
		istack_destroy(stack);
	}	
}

static void __performance_test() {
	log_printline("add find remove performance test start");
	log_printline("add find remove performance test end");
}

void cntr2_odlist_test() {
	test_run_single("cntr cntr_base correctness", __correctness_test);

	test_run_bench("cntr cntr_base performance", __performance_test);
}
