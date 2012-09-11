#ifndef _CNTR_ALGORITHM_H_
#define _CNTR_ALGORITHM_H_

#include <citer_base.h>

/* citer related start */
extern inline void citer_swap(citer first, citer second);

extern inline bool citer_equal(citer first, citer second);

extern inline void citer_assign(citer first, citer second);

typedef void (*pf_for_each_process)(citer itr);

void citer_for_each(citer begin, citer end, pf_for_each_process proc);

typedef void (*pf_for_each_process_v)(citer itr, void* param);

void citer_for_each_v(citer begin, citer end, pf_for_each_process_v proc, void* param);



/* sorting algorithm.  */
typedef int (*pf_compare_object)(const void*, const void*);

void quick_sort(citer begin, citer end, pf_compare_object comp);

void bubble_sort(citer begin, citer end, pf_compare_object comp);

void merge_sort(citer begin, citer end, pf_compare_object comp);

/* common functions for linear container */
void reverse(citer begin, citer end);

bool prev_permutation(citer begin, citer end, pf_compare_object comp);

bool next_permutation(citer begin, citer end, pf_compare_object comp);

/* cntr related start */
bool cntr_equal(cntr first, cntr second);

void cntr_swap(cntr first, cntr second);

void cntr_assign(cntr first, cntr second);

void cntr_sort(cntr c);




typedef bool (*pf_find_accept)(void* object);

void find_largest(citer begin, citer end, __out citer result, pf_compare_object comp);

void find_smallest(citer begin, citer end, __out citer result, pf_compare_object comp);

bool find_first(citer begin, citer end, __out citer result, pf_find_accept accept);

bool find_last(citer begin, citer end, __out citer result, pf_find_accept accept);


#endif /* _CNTR_ALGORITHM_H_ */
