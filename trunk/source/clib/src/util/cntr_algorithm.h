#ifndef _CNTR_ALGORITHM_H_
#define _CNTR_ALGORITHM_H_

#include <cntr_iterator.h>

#define for_each(CNTR_TYPE, CNTR_NAME, ITERATOR_NAME) \
	for (citer (ITERATIO_NAME), CNTR_TYPE##_citer_begin(&(CNTR_NAME), &(ITERATOR_NAME)); \
			citer_valid(&(ITERATOR_NAME)); \
			citer_to_next(&(ITERATOR_NAME)))

void citer_swap(citer* first, citer* second);

typedef int (*pf_compare_object)(void*, void*);
void quick_sort(citer* begin, citer* end, pf_compare_object comp);

void bubble_sort(citer* begin, citer* end, pf_compare_object comp);

typedef bool (*pf_find_accept)(void* object, void* param);
typedef struct cntr_find_criterion {
	pf_find_accept accept;
	void*          param;
} cntr_fc;

bool find_first(citer* begin, citer* end, citer* result, cntr_fc* criterion);
bool find_last(citer* begin, citer* end, citer* result, cntr_fc* criterion);


#endif /* _CNTR_ALGORITHM_H_ */
