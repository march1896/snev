/*
 * Main test file
 */

#include <test_util.h>
#include <stdio.h>
#include <memheap/heap_global.h>

extern void heap_llrb_test();
extern void heap_sysd_test();

extern void heap_test();
extern void cntr_linear_test();
extern void cntr_sort_test();
extern void cntr_base_test();
extern void algorithm_base_test();
extern void cntr2_odlist_test();

extern void llrb_link_test();

static void debug_global_leak() {
#ifdef _MEM_DEBUG_
#endif
}

void run_all_test() {
	//do_test("heap test", heap_test);
	test_run_single("heap_llrb test", heap_llrb_test);
	test_run_single("heap_sysd test", heap_sysd_test);

	heap_global_init();
	//do_test("llrb link", llrb_link_test);
	test_run_single("cntr2 test", cntr2_odlist_test);
	debug_global_leak();
 	test_run_single("algorithm test", algorithm_base_test);
 	debug_global_leak();
 	test_run_single("cntr linear ALL", cntr_base_test);
 	debug_global_leak();
 	test_run_single("sort algorithm", cntr_sort_test);
 	debug_global_leak();
	heap_global_deinit();
// 	heap_deinit_global();
}

#include <cntr/cntr_test_util.h>
void main() {
	init_data();
	run_all_test();
	destroy_data();
}
