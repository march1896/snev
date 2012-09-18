/*
 * Main test file
 */

#include <test_util.h>
#include <stdio.h>

extern void cntr_linear_test();
extern void cntr_sort_test();
extern void cntr_base_test();
extern void algorithm_base_test();

extern void llrb_link_test();

void run_all_test() {
	do_test("llrb link", llrb_link_test);

	do_test("algorithm test", algorithm_base_test);
	do_test("cntr linear ALL", cntr_base_test);
	do_test("sort algorithm", cntr_sort_test);
}

void main() {
	init_data();
	run_all_test();
	destroy_data();
}
