#ifndef _CNTR_ALGORITHM_H_
#define _CNTR_ALGORITHM_H_

#include <cntr_iterator.h>

/* citer related start */
extern inline void citer_swap(citer first, citer second);

extern inline bool citer_equal(citer first, citer second);

extern inline void citer_assign(citer first, citer second);

typedef void (*pf_for_each_process)(citer itr);

void citer_for_each(citer begin, citer end, pf_for_each_process proc);


/* cntr related start */
bool cntr_equal(cntr first, cntr second);

void cntr_swap(cntr first, cntr second);

void cntr_assign(cntr first, cntr second);

/* sorting algorithm.  */
typedef int (*pf_compare_object)(void*, void*);

void quick_sort(citer begin, citer end, pf_compare_object comp);

void bubble_sort(citer begin, citer end, pf_compare_object comp);



typedef bool (*pf_find_accept)(void* object);

void find_largest(citer begin, citer end, __out citer result, pf_compare_object comp);

void find_smallest(citer begin, citer end, __out citer result, pf_compare_object comp);

bool find_first(citer begin, citer end, __out citer result, pf_find_accept accept);

bool find_last(citer begin, citer end, __out citer result, pf_find_accept accept);


#endif /* _CNTR_ALGORITHM_H_ */
