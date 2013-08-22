#include <stdio.h>
#include <string.h>

#include "cntr2/ilist.h"
#include "cntr2/ifactory.h"

#include "test_util.h"
#include "cntr2/ilist.test.h"

// remove this 
#include "cntr2/test.h"

static void __correctness_test() {
	object* dlist = create_dblinked_list();

	list_test_basic(as_list(dlist));

	queue_correct(as_queue(dlist));
	stack_correct(as_stack(dlist));
	set_correct(as_set(dlist));

	ilist_destroy(as_list(dlist));
}

static void __performance_test() {
	log_printline("add find remove performance test start");
	log_printline("add find remove performance test end");
}

void cntr2_odlist_test() {
	test_run_single("cntr cntr_base correctness", __correctness_test);

	test_run_single("cntr cntr_base performance", __performance_test);
}
