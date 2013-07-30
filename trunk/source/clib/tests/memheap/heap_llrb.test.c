#include <memheap/heap_factory.h>

#include <heap_util.test.h>

void heap_llrb_correct_test() {
	struct heap_llrb* testheap = heap_llrb_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);

	heaptest_set_heap(testheap, heap_llrb_alloc, heap_llrb_dealloc);
	/* fill all data for correctness */
	heaptest_set_debugstate(true, false);

	heaptest_run_allcomb(100);
	
	heap_llrb_join(testheap);
}

void heap_llrb_performance_test() {
	struct heap_llrb* testheap = heap_llrb_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
			);

	heaptest_set_heap(testheap, heap_llrb_alloc, heap_llrb_dealloc);
	heaptest_set_debugstate(false, true);

	heaptest_run_allcomb(10000);
	
	heap_llrb_join(testheap);
}
