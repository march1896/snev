//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cntr2/oallocator.h>
#include <memheap/heap_walk.h>

#include "test_util.h"

static void allocator_trace_test() {
	void* addr[10];
	int size[10], i;

	for (i = 0; i < 10; i ++) {
		size[i] = (i + 1) * 10;
		addr[i] = (void*)allocator_alloc(__global_sysd_allocator, size[i]);
		memset(addr[i], size[i], 0);
	}

	for (i = 0; i < 10; i ++) {
		allocator_dealloc(__global_sysd_allocator, addr[i]);
	}

	{
		allocator allrb;
		allrb = allocator_llrb_spawn(__global_sysd_allocator);
		for (i = 0; i < 10; i ++) {
			size[i] = (i + 1) * 10;
			addr[i] = (void*)allocator_alloc(allrb, size[i]);
			memset(addr[i], size[i], 0);
		}

		for (i = 0; i < 10; i ++) {
			if (i == 5 || i == 8) {
				/* two memory leak point */
				continue;
			}
			allocator_dealloc(allrb, addr[i]);
		}

		allocator_walk(allrb, allocator_heap_walk_print, NULL);
		log_printline("two leak pointer expected");

		allocator_join(allrb);
	}
	
	return;
}

typedef allocator (*pf_spawn_cb)(allocator parent);

void cntr2_allocator_recursive_spawn_test() {
	allocator grand_father = allocator_llrb_spawn(__global_sysd_allocator);
	allocator father       = allocator_llrb_spawn(grand_father);
	allocator uncle        = allocator_llrb_spawn(grand_father);
	allocator brother      = allocator_llrb_spawn(father);
	allocator blade        = allocator_llrb_spawn(father);
	/* blade's family */

	int i;
	void* addr[10];

	/* brother alloc/dealloc all element */
	for (i = 0; i < 10; i ++) {
		addr[i] = allocator_alloc(brother, i * 10 + 1);
	}
	for (i = 0; i < 10; i ++) {
		allocator_dealloc(brother, addr[i]);
	}

	/* blade has two memory leak */
	for (i = 0; i < 10; i ++) {
		addr[i] = allocator_alloc(blade, i * 10 + 1);
	}
	for (i = 0; i < 10; i ++) {
		if (i != 5 && i != 7) {
			allocator_dealloc(blade, addr[i]);
		}
	}

	/* father has one memory leak */
	allocator_alloc(father, 10);

	/* no operations happened on uncle */
	log_printline("before parent is joined");
	log_printline("%s leak detect:", "grand_father");
	allocator_walk(grand_father, allocator_heap_walk_print, NULL);
	log_printline("%s leak detect:", "father");
	allocator_walk(father, allocator_heap_walk_print, NULL);
	log_printline("%s leak detect:", "uncle");
	allocator_walk(uncle, allocator_heap_walk_print, NULL);
	log_printline("%s leak detect:", "brother");
	allocator_walk(brother, allocator_heap_walk_print, NULL);
	log_printline("%s leak detect:", "blade");
	allocator_walk(blade, allocator_heap_walk_print, NULL);

	allocator_join(father);
	log_printline("after parent is joined");
	log_printline("%s leak detect:", "grand_father");
	allocator_walk(grand_father, allocator_heap_walk_print, NULL);
	/*
	log_printline("%s leak detect:\n", "father");
	allocator_walk(father, heap_leak_print_to_terminal, NULL);
	*/
	log_printline("%s leak detect:", "uncle");
	allocator_walk(uncle, allocator_heap_walk_print, NULL);
	/*
	log_printline("%s leak detect:\n", "brother");
	allocator_walk(brother, heap_leak_print_to_terminal, NULL);
	log_printline("%s leak detect:\n", "blade");
	allocator_walk(blade, heap_leak_print_to_terminal, NULL);
	*/

	allocator_join(grand_father);
}

void cntr2_oallocator_test() {
	test_run_single("allocator simple trace test", allocator_trace_test);

	// this test will add some leak info to the stdout.
	// test_run_single("allocator recursive spawn test", cntr2_allocator_recursive_spawn_test);
}
