#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#include <time.h>

#define do_test(test_name, test_function) \
	printf("%s test start\n", test_name);\
	test_function(); \
	printf("%s test end\n\n", test_name);


#endif /* _TEST_UTIL_H_ */
