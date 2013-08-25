#include "memheap/heap_global.h"
#include "cntr2/oallocator.h"

#include "test_util.h"

extern void cntr2_oallocator_test();
extern void cntr2_odlist_test();
extern void cntr2_ollrb_test();

void cntr2_all_test() {
	test_run_single("allocator object test", cntr2_oallocator_test);

	test_run_single("dlist object test", cntr2_odlist_test);

	test_run_single("ollrb object test", cntr2_ollrb_test);

	/* check the container/iterator memory leak */
	allocator_walk(global_llrb_allocator, allocator_heap_walk_print, NULL);
}
