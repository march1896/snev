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
	heap_pagesize_small,        /* smaller than 1/20 of HEAP_MINIMUM_EXPAND_SIZE */
	heap_pagesize_middle,       /* between 1/20 and 1/4 of HEAP_MINIMUM_EXPAND_SIZE */
	heap_pagesize_large,        /* between 1/4 and 1 HEAP_MINIMUM_EXPAND_SIZE */
	heap_pagesize_huge,         /* between 1 and 5 HEAP_MINIMUM_EXPAND_SIZE */
	heap_pagesize_random,       /* totally random */
	heap_pagesize_count
};

enum heaptest_page_pattern {
	heap_pagepattern_unique,           /* all pages are the same */
	heap_pagepattern_increasing,       /* page size is increasing */
	heap_pagepattern_decreasing,       /* page size is decreasing */
	heap_pagepattern_jag,              /* page size is jagged */
	heap_pagepattern_random_patterned, /* random page size, but size is pattern, in other words, not too much different king of size */
	heap_pagepattern_random,           /* totally random page size */
	heap_pagepattern_special0,

	heap_pagepattern_count
};

void heaptest_begin();
void heaptest_end();

void heaptest_set_heap(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc);
void heaptest_set_heap_plus(int split_threshold, int expand_size);

void heaptest_set_filldata(bool filldata);
void heaptest_set_logdata(bool log, const char* file);
/* format 0 is simplified, 1 is verbosed */
void heaptest_set_logtime(bool log, int format, const char* file);

void heaptest_run_single(int __length, enum heaptest_op_order __order, enum heaptest_page_size __pagesize, enum heaptest_page_pattern __pagepattern);

void heaptest_run_allcomb(int __length);

#endif /* _HEAP_TEST_UTILITY_H_ */
