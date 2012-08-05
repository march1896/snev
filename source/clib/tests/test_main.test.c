/*
 * Main test file
 */

extern void cntr_linear_test();
extern void cntr_sort_test();

void run_all_test() {
	cntr_linear_test();
	cntr_sort_test();
}

void main() {
	run_all_test();
}
