/*
 * Main test file
 */

#include <test_util.h>
#include <stdio.h>

extern void heap_test();
extern void cntr_linear_test();
extern void cntr_sort_test();
extern void cntr_base_test();
extern void algorithm_base_test();
extern void cntr2_odlist_test();

extern void llrb_link_test();

static void debug_global_leak() {
#ifdef _MEM_DEBUG_
	heap_debug_global_leak();
#endif
}

void run_all_test() {
	//do_test("heap test", heap_test);

	heap_init_global(128 * 1024 * 1024);
	//do_test("llrb link", llrb_link_test);
	do_test("cntr2 test", cntr2_odlist_test);
	debug_global_leak();
	do_test("algorithm test", algorithm_base_test);
	debug_global_leak();
	do_test("cntr linear ALL", cntr_base_test);
	debug_global_leak();
	do_test("sort algorithm", cntr_sort_test);
	debug_global_leak();
	heap_deinit_global();
}

void main() {
	init_data();
	run_all_test();
	destroy_data();
}
