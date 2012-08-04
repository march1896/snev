#ifndef _CNTR_ALGORITHM_H_
#define _CNTR_ALGORITHM_H_

#include <cntr_iterator.h>

/*
 * ITERATOR_ADDR MUST BE declared before 
 */

extern inline void citer_swap(citer first, citer second);

extern inline bool citer_equal(citer first, citer second);

extern inline void citer_assign(citer first, citer second);

typedef void (*pf_for_each_process)(citer itr);

void citer_for_each(citer begin, citer end, pf_for_each_process proc);
/*
 * sorting algorithm.
 */
typedef int (*pf_compare_object)(void*, void*);
void quick_sort(citer begin, citer end, pf_compare_object comp);

void bubble_sort(citer begin, citer end, pf_compare_object comp);

typedef bool (*pf_find_accept)(void* object, void* param);
typedef struct cntr_find_criterion {
	pf_find_accept accept;
	void*          param;
} cntr_fc;

bool find_first(citer begin, citer end, citer result, cntr_fc* criterion);
bool find_last(citer begin, citer end, citer result, cntr_fc* criterion);


#endif /* _CNTR_ALGORITHM_H_ */
