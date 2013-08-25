#include "test_util.h"

extern void util_all_test();
extern void heap_all_test();
extern void cntr2_all_test();
extern void cntr_all_test();

int main() {
	test_run_single("util all test", util_all_test);
	test_run_single("heap all test", heap_all_test);

	test_run_single("cntr2 all test", cntr2_all_test);

	/* deprecated */
	//test_run_single("cntr all test", cntr_all_test);
}
