#include <test_util.h>

extern void heap_llrb_test();
extern void heap_sysd_test();

void heap_all_test() {
	test_run_single("heap_llrb test", heap_llrb_test);

	test_run_single("heap_sysd test", heap_sysd_test);
}

