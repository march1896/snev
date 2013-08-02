#ifndef _TEST_UTILITIES_H_
#define _TEST_UTILITIES_H_
#include <cominc.h>

typedef void (*pf_test_case)();

void test_run_single(const char* test_name, pf_test_case test_func);

void test_log_use_file(const char* file_name);
void test_log_use_header(bool use_or_not);
void test_log_use_footer(bool use_or_not);
void test_log_time(bool log);

#endif /* _TEST_UTILITIES_H_ */
