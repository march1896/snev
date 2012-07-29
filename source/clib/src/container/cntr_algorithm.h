#ifndef _CNTR_ALGORITHM_H_
#define _CNTR_ALGORITHM_H_

#define for_each(CNTR_TYPE, CNTR_NAME ITERATOR_NAME) \
	for (citer ITERATIO_NAME, CNTR_NAME##_citer_begin(&CNTR_NAME, &ITERATOR_NAME); \
			citer_valid(&ITERATOR_NAME); \
			citer_to_next(&ITERATOR_NAME))

void quick_sort(citer* begin, citer* end);

void bubble_sort(citer* begin, citer* end);

#endif /* _CNTR_ALGORITHM_H_ */
