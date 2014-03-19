#ifndef _TEST_UTILITIES_H_
#define _TEST_UTILITIES_H_
#include <util/cominc.h>
#include <stdint.h>

typedef void (*pf_test_case)();
typedef void (*pf_test_case_v)(void* target, void* context);

void log_inc_tab(bool forward);
void log_printf(const char* format, ...);
void log_printtab();
void log_printline(const char* format, ...);

void test_run_single(const char* test_name, pf_test_case test_func);
void test_run_bench(const char* test_name, pf_test_case test_func);
void test_run_feature(const char* test_name, pf_test_case test_func);

void test_run_single_v(const char* test_name, pf_test_case_v test_func, void* target, void* context);
void test_run_bench_v(const char* test_name, pf_test_case_v test_func, void* target, void* context);
void test_run_feature_v(const char* test_name, pf_test_case_v test_func, void* target, void* context);

void test_log_use_file(const char* file_name);
void test_log_use_header(bool use_or_not);
void test_log_use_footer(bool use_or_not);
void test_log_time(bool log);

struct heap_blockinfo;
void allocator_heap_walk_print(struct heap_blockinfo* binfo, void* param);

extern bool        g_use_file;
extern const char* g_log_file;
extern bool        g_log_header;
extern bool        g_log_footer;
extern bool        g_log_time;

extern const char  g_table_char;
extern int         g_table_step;
extern int         g_table_length;

#endif /* _TEST_UTILITIES_H_ */
