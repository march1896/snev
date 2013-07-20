#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#include <time.h>
#include <cntr.h>

#define do_test(test_name, test_function) \
	printf("%s test start\n", test_name);\
	test_function(); \
	printf("%s test end\n\n", test_name);

int cntr_int_compare(const void* x, const void* y);

int qsort_int_compare(const void* x, const void* y);

// correctness_data_length;
#define CORRECTNESS_LENGTH 300
// performance_data_length;
#define PERFORMANCE_LENGTH 500000
extern int *rawdata, *uniquedata, *uniquesorteddata;

void init_data();

void destroy_data();

typedef enum test_cont_type_e {
	TC_LIST = 0,
	TC_ARRAY,
	TC_BST,
	TC_RBT,
	TC_234T,
	TC_LLRBTREE,
	TC_END
} TC_TYPE;

typedef enum test_data_type_e {
	TD_INCREASE = 0,
	TD_DECREASE,
	TD_RANDOM,
	TD_UNIQUE,
	TD_END
} TD_TYPE;

typedef enum test_data_length_e {
	TL_CORRECTNESS = 0,
	TL_PERFORMANCE,
	TL_END
} TD_LENGTH;

const char* cntr_name(TC_TYPE ct);

const char* data_order_name(TD_TYPE dt);

void generate_test_data(TD_TYPE dt, TD_LENGTH dl, int* length, int* ulength);

cntr cntr_create(TC_TYPE ct);


#endif /* _TEST_UTIL_H_ */
