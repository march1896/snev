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
#define clength 300
// performance_data_length;
#define plength 10000
extern int *rawdata, *uniquedata, *uniquesorteddata;

void init_data();

void destroy_data();

typedef enum test_cont_type_e {
	ec_list = 0,
	ec_array,
	ec_bst,
	ec_end
} test_cont_type;

typedef enum test_data_type_e {
	ed_increase = 0,
	ed_decrease,
	ed_random,
	ed_end
} test_data_type;

typedef enum test_data_length_e {
	el_correctness = 0,
	el_performance,
	el_end
} test_data_length;

const char* cntr_name(test_cont_type ct);

const char* data_order_name(test_data_type dt);

void generate_test_data(test_data_type dt, test_data_length dl, int* length, int* ulength);

cntr cntr_create(test_cont_type ct);


#endif /* _TEST_UTIL_H_ */
