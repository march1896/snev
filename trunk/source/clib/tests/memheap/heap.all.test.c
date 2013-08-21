#include <test_util.h>

#include <memheap/heap_util.test.h>

extern void heap_llrb_test();
extern void heap_buddy_test();
extern void heap_sysd_test();

bool enable_heap_bench = false;

void heap_all_test() {
	/* NOTE, this test is not very accurate, since I have run a lot of the this 
	 * test on my machine, with the same build environment. 
	 * But the result is quite different, currently, I guess the system heap's 
	 * allocation speed is not stable, it will very from time to time, I don't 
	 * know why. */

	test_run_single("heap_llrb test", heap_llrb_test);

	test_run_single("heap_buddy test", heap_buddy_test);

	test_run_single("heap_sysd test", heap_sysd_test);
}

