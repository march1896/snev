#ifndef _HEAP_TEST_UTILITY_H_
#define _HEAP_TEST_UTILITY_H_

#include <memheap/heap_def.h>

/* alloc/dealloc operation order */
enum heaptest_op_order {
	heap_op_increasing, /* first alloc all data, then dealloc them in the same order */
	heap_op_decreasing, /* first alloc all data, then dealloc them in reverse order */
	heap_op_alternately,/* alloc dealloc alternately */
	heap_op_random,     /* totally random order */
	heap_op_patterned0, 
	heap_op_patterned1,
	heap_op_patterned2,

	heap_op_order_count
};

enum heaptest_page_size {
	heap_pagesize_small,
	heap_pagesize_middle,
	heap_pagesize_large,

	heap_pagesize_count
};

enum heaptest_page_pattern {
	heap_pagepattern_unique,     /* all pages are the same */
	heap_pagepattern_increasing, /* page size is increasing */
	heap_pagepattern_decreasing, /* page size is decreasing */
	heap_pagepattern_jag,        /* page size is jagged */
	heap_pagepattern_random_patterned, /* random page size, but size is pattern, in other words, not too much different king of size */
	heap_pagepattern_random,     /* totally random page size */
	heap_pagepattern_special0,

	heap_pagepattern_count
};

void heaptest_set_heap(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc);
void heaptest_set_debugstate(bool filldata, bool logtime);

void heaptest_run_single(int __length, enum heaptest_op_order __order, enum heaptest_page_size __pagesize, enum heaptest_page_pattern __pagepattern);

void heaptest_fun_allcomb(int __length);

#endif /* _HEAP_TEST_UTILITY_H_ */
