#ifndef _CNTR2_TEST_ELEMENTDATA_H_
#define _CNTR2_TEST_ELEMENTDATA_H_

#include <cntr2/oo_model.h>

/* module test data */
#define TD_NUM 100000
#define TD_STRING_MAX_LENGTH 30
/*
extern address int_repo[TD_NUM];
extern address longint_repo[TD_NUM];
extern address float_repo[TD_NUM];
extern address double_repo[TD_NUM];
extern address char_repo[TD_NUM];
extern address string_repo[TD_NUM];
*/

struct test_data_desc {
	address        *data_repo;
	unknown_traits *data_traits; 
};

extern struct test_data_desc int_test_desc;
extern struct test_data_desc longint_test_desc;
extern struct test_data_desc float_test_desc;
extern struct test_data_desc double_test_desc;
extern struct test_data_desc char_test_desc;
extern struct test_data_desc string_test_desc;

void td_repo_init();
void td_repo_deinit();

#endif /* _CNTR2_TEST_ELEMENTDATA_H_ */
