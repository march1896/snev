#include <test_util.h>

extern void llrb_link_test();
extern void splay_link_test();
extern void skiplist_test();

void util_all_test() {
	test_run_single("llrb_link test", llrb_link_test);

	test_run_single("splay_link test", splay_link_test);

	test_run_single("skiplist test", skiplist_test);
}

