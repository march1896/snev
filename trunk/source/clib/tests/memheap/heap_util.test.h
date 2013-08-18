#ifndef _HEAP_TEST_UTILITY_H_
#define _HEAP_TEST_UTILITY_H_

#include <memheap/heap_def.h>

/* alloc/dealloc operation order */
enum ht_op_order {
	ht_op_inorder,      /* first alloc all data, then dealloc them in the same order */
	hr_op_reverseorder, /* first alloc all data, then dealloc them in reverse order */
	ht_op_alternately,  /* alloc dealloc alternately */
	ht_op_random,       /* totally random order */
	/*
	ht_op_patterned0, 
	ht_op_patterned1,
	ht_op_patterned2,
	*/

	ht_op_order_count
};

enum ht_data_size {
	ht_data_small,        /* smaller than 1/20 of HEAP_MINIMUM_EXPAND_SIZE */
	ht_data_middle,       /* between 1/20 and 1/4 of HEAP_MINIMUM_EXPAND_SIZE */
	ht_data_large,        /* between 1/4 and 1 HEAP_MINIMUM_EXPAND_SIZE */
	ht_data_huge,         /* between 1 and 5 HEAP_MINIMUM_EXPAND_SIZE */
	ht_data_random,       /* totally random */
	ht_data_count
};

enum ht_data_pattern {
	ht_pattern_unique,           /* all pages are the same */
	ht_pattern_increasing,       /* page size is increasing */
	ht_pattern_decreasing,       /* page size is decreasing */
	ht_pattern_jag,              /* page size is jagged */
	ht_pattern_random_restricted, /* random page size, but size is pattern, in other words, not too much different king of size */
	ht_pattern_random,           /* totally random page size */
	/*
	ht_pattern_special0,
	ht_pattern_special1,
	ht_pattern_special2,
	*/

	ht_pattern_count
};

void heaptest_begin();
void heaptest_end();

void heaptest_set_heap(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc);

/* load the heap with some data before test, first alloc alloc_count with the currently active data type, then randomly release dealloc_count of them. */
void heaptest_set_preload(int alloc_count, int dealloc_count);
/* normally, we don't need to fill data if we are testing performance */
void heaptest_set_filldata(bool filldata);

void heaptest_set_logoperation(bool log, const char* file);
/* format 0 is simplified, 1 is verbose */
void heaptest_set_logtime(bool log, int format, const char* file);

void heaptest_run_single(int __length, enum ht_op_order __order, enum ht_data_size __pagesize, enum ht_data_pattern __pagepattern);

void heaptest_run_allcomb(int __length);

extern bool enable_heap_bench;

#endif /* _HEAP_TEST_UTILITY_H_ */
