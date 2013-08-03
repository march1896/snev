#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oallocator.h>

#include <test_util.h>

static void allocator_trace_test() {
	void* addr[10];
	int size[10], i;
	for (i = 0; i < 10; i ++) {
		size[i] = (i + 1) * 10;
		addr[i] = (void*)allocator_alloc(default_allocator, size[i]);
		memset(addr[i], size[i], 0);
	}

	for (i = 0; i < 10; i ++) {
		allocator_dealloc(default_allocator, addr[i]);
	}

	return;
}

void cntr2_oallocator_test() {
	test_run_single("allocator simple trace test", allocator_trace_test);
}
