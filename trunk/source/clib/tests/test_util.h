#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#include <time.h>

#define do_test(test_name, test_function) \
	printf("%s test start\n", test_name);\
	test_function(); \
	printf("%s test end\n\n", test_name);

int cntr_int_compare(const void* x, const void* y);

int qsort_int_compare(const void* x, const void* y);

// correctness_data_length;
#define clength 300
// performance_data_length;
#define plength 10000
extern int *x, *y;

void init_data();

void destroy_data();

int generate_data(int type, int length);

#endif /* _TEST_UTIL_H_ */
