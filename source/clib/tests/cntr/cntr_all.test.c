/*
 * Main test file
 */

#include <test_util.h>
#include <stdio.h>

#include <memheap/heap_global.h>

#include <cntr/cntr_test_util.h>

extern void cntr_linear_test();
extern void cntr_sort_test();
extern void cntr_base_test();
extern void algorithm_base_test();

static void debug_global_leak() {
#ifdef _MEM_DEBUG_
#endif
}

void cntr_all_test() {
	init_data();

	heap_global_init();

	debug_global_leak();
 	test_run_single("algorithm test", algorithm_base_test);
 	debug_global_leak();

 	test_run_single("cntr linear ALL", cntr_base_test);
 	debug_global_leak();

 	test_run_single("sort algorithm", cntr_sort_test);
 	debug_global_leak();

	heap_global_deinit();

	destroy_data();
}
