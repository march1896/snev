#include <memheap/heap_factory.h>
#include <memheap/heap_sys.h>
#include <memheap/heap_global.h>

#include <memheap/heap_util.test.h>
#include <test_util.h>

void heap_sysd_correct_test() {
	heaptest_begin();
	heaptest_set_heap(__global_heap_sysd, heap_sysd_alloc, heap_sysd_dealloc);
	heaptest_set_filldata(true);

	heaptest_run_single(100, hr_op_reverseorder, ht_data_small, ht_pattern_unique);
	heaptest_run_allcomb(100);

	heaptest_end();
}

void heap_sysd_performance_test() {
	heaptest_set_heap(__global_heap_sysd, heap_sysd_alloc, heap_sysd_dealloc);
	heaptest_set_filldata(false);
	heaptest_set_logtime(true, 0, "heap_sysd_performance.txt");

	heaptest_run_allcomb(10000);
}

void heap_sysd_test() {
	test_run_single("heap_sysd correctness test", heap_sysd_correct_test);

	test_run_single("heap_sysd performance test", heap_sysd_performance_test);
}
