#include <memheap/heap_factory.h>
#include <memheap/heap_pool.h>
#include <memheap/heap_global.h>
#include <memheap/heap_walk.h>

#include <memheap/heap_util.test.h>

#include <test_util.h>

static void heap_spool_correct_test() {
	struct heap_spool* testheap = heap_spool_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);

	heaptest_begin();
	heaptest_set_heap((void*)testheap, (pf_alloc)heap_spool_alloc, (pf_dealloc)heap_spool_dealloc);

	heaptest_set_filldata(true);
	heaptest_run_single(100, ht_op_inorder, ht_data_small, ht_pattern_unique);
	heaptest_run_single(100, ht_op_reverseorder, ht_data_small, ht_pattern_unique);
	heaptest_run_single(100, ht_op_alternately, ht_data_small, ht_pattern_unique);
	heaptest_run_single(100, ht_op_random, ht_data_small, ht_pattern_unique);

	heaptest_end();

	/*
	{
		struct heap_state cur_state;
		heap_state_init(&cur_state);
		heap_spool_walk(testheap, heap_query_state, &cur_state);
	}

	heap_spool_walk(testheap, heap_leak_print_to_terminal, NULL);
	*/
	
	heap_spool_join(testheap);
}

static void heap_spool_performance_test() {
	struct heap_spool* testheap = heap_spool_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);

	heaptest_begin();
	heaptest_set_heap((void*)testheap, (pf_alloc)heap_spool_alloc, (pf_dealloc)heap_spool_dealloc);

	heaptest_set_filldata(true);
	heaptest_run_single(10000, ht_op_inorder, ht_data_small, ht_pattern_unique);
	heaptest_run_single(10000, ht_op_reverseorder, ht_data_small, ht_pattern_unique);
	heaptest_run_single(10000, ht_op_alternately, ht_data_small, ht_pattern_unique);
	heaptest_run_single(10000, ht_op_random, ht_data_small, ht_pattern_unique);

	heaptest_end();

	/*
	{
		struct heap_state cur_state;
		heap_state_init(&cur_state);
		heap_spool_walk(testheap, heap_query_state, &cur_state);
	}

	heap_spool_walk(testheap, heap_leak_print_to_terminal, NULL);
	*/
	
	heap_spool_join(testheap);
}

void heap_spool_test() {
	test_run_single("heap_spool correctness test", heap_spool_correct_test);

	if (enable_heap_bench) {
		test_run_single("heap_spool performance test", heap_spool_performance_test);
	}
}

static void heap_mpool_correct_test() {
	struct heap_mpool* testheap = heap_mpool_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);

	heaptest_begin();
	heaptest_set_heap((void*)testheap, (pf_alloc)heap_mpool_alloc, (pf_dealloc)heap_mpool_dealloc);

	heaptest_set_filldata(true);
	heaptest_run_single(100, ht_op_inorder, ht_data_small, ht_pattern_unique);
	heaptest_run_single(100, ht_op_reverseorder, ht_data_small, ht_pattern_unique);
	heaptest_run_single(100, ht_op_alternately, ht_data_small, ht_pattern_unique);
	heaptest_run_single(100, ht_op_random, ht_data_small, ht_pattern_unique);

	heaptest_end();

	/*
	{
		struct heap_state cur_state;
		heap_state_init(&cur_state);
		heap_mpool_walk(testheap, heap_query_state, &cur_state);
	}

	heap_mpool_walk(testheap, heap_leak_print_to_terminal, NULL);
	*/
	
	heap_mpool_join(testheap);
}
