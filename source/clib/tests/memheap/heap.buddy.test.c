#include <memheap/heap_factory.h>
#include <memheap/heap_sys.h>
#include <memheap/heap_global.h>
#include <memheap/heap_walk.h>

#include <memheap/heap_util.test.h>

#include <test_util.h>

void heap_buddy_correct_test() {
	struct heap_buddy* testheap = heap_buddy_spawn(
				__global_sysd_heap, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);
	heaptest_begin();
	heaptest_set_heap((void*)testheap, (pf_alloc)heap_buddy_alloc, (pf_dealloc)heap_buddy_dealloc);
	heaptest_set_filldata(true);

	heaptest_run_single(100, ht_op_reverseorder, ht_data_small, ht_pattern_unique);
	heaptest_run_allcomb(1000);

	heaptest_end();

	{
		struct heap_state cur_state;
		heap_state_init(&cur_state);
		heap_buddy_walk(testheap, heap_query_state, &cur_state);
	}

	heap_buddy_walk(testheap, heap_leak_print_to_terminal, NULL);
	
	heap_buddy_join(testheap);
}

void heap_buddy_performance_test() {
	struct heap_buddy* testheap = heap_buddy_spawn(
				__global_sysd_heap, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);

	heaptest_begin();
	heaptest_set_heap((void*)testheap, (pf_alloc)heap_buddy_alloc, (pf_dealloc)heap_buddy_dealloc);
	heaptest_set_filldata(false);
	heaptest_set_logtime(true, 0, "heap_buddy_performance.txt");

	heaptest_run_allcomb(40000);
	heaptest_end();
	
	heap_buddy_join(testheap);
}

void heap_buddy_test() {
	test_run_single("heap_buddy correctness test", heap_buddy_correct_test);

	test_run_bench("heap_buddy performance test", heap_buddy_performance_test);
}
