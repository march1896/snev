#include <test_util.h>

extern void llrb_link_test();

void util_all_test() {
	test_run_single("llrb_link test", llrb_link_test);
}

