#include <test_util.h>

extern void cntr2_oallocator_test();
extern void cntr2_odlist_test();

void cntr2_all_test() {
	test_run_single("allocator object test", cntr2_odlist_test);

	test_run_single("dlist object test", cntr2_odlist_test);
}
