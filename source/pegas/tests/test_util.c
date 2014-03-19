#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "memheap/heap_def.h"

#include "test_util.h"

bool        g_use_file = false;
const char* g_log_file = "test_log.txt";
bool        g_log_header = true;
bool        g_log_footer = true;
bool        g_log_time = true;

const char  g_table_char = ' ';
int         g_table_step = 2;
int         g_table_length = 0;

static char g_table_buff[101];

void log_inc_tab(bool inc) {
	int i;

	if (inc == true) {
		g_table_length += g_table_step;
		if (g_table_length >= 100) 
			g_table_length = 100;
	}
	else {
		g_table_length -= g_table_step;
		if (g_table_length < 0) 
			g_table_length = 0;
	}

	for (i = 0; i < g_table_length; i ++)
		g_table_buff[i] = g_table_char;

	g_table_buff[g_table_length] = '\0';
}

void log_printf(const char* format, ...) {
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
    va_end(arglist);
	if (g_use_file) {
		FILE * pFile = fopen(g_log_file, "a");
		va_start(arglist, format);
		vfprintf(pFile, format, arglist);
		fclose(pFile);
        va_end(arglist);
	}
}

void log_printtab() {
	printf("%s", g_table_buff);
	if (g_use_file) {
		FILE * pFile = fopen(g_log_file, "a");
		fprintf(pFile, "%s", g_table_buff);
		fclose(pFile);
	}
}

void log_printline(const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
    
	printf("%s", g_table_buff);
	vprintf(format, arglist);
	printf("\n");
    va_end(arglist);
    
	if (g_use_file) {
		FILE * pFile = fopen(g_log_file, "a");
		va_start(arglist, format);
		fprintf(pFile, "%s", g_table_buff);
		vfprintf(pFile, format, arglist);
		fprintf(pFile, "\n");
		fclose(pFile);
        va_end(arglist);
	}
	
}

void test_run_single(const char* test_name, pf_test_case test_func) {
	clock_t start_c, end_c;
	start_c = clock();

	if (test_name == NULL) {
		test_name = "unknown test";
	}

	log_inc_tab(true);

	if (g_log_header) {
		log_printline("[%s begins]", test_name);
	}

	test_func();

	end_c = clock();

	if (g_log_footer) {
		if (g_log_time) 
			log_printline("[%s ends, total time use %f]", test_name, (float)(end_c - start_c)/CLOCKS_PER_SEC);
		else 
			log_printline("[%s ends]", test_name);
	}

	log_inc_tab(false);
}

void test_run_single_v(const char* test_name, pf_test_case_v test_func_v, void* target, void* context) {
	clock_t start_c, end_c;
	start_c = clock();

	if (test_name == NULL) {
		test_name = "unknown test";
	}

	log_inc_tab(true);

	if (g_log_header) {
		log_printline("[%s begins]", test_name);
	}

	test_func_v(target, context);

	end_c = clock();

	if (g_log_footer) {
		if (g_log_time) 
			log_printline("[%s ends, total time use %f]", test_name, (float)(end_c - start_c)/CLOCKS_PER_SEC);
		else 
			log_printline("[%s ends]", test_name);
	}

	log_inc_tab(false);
}

void test_run_bench(const char* test_name, pf_test_case test_func) {
	bool old_log_time = g_log_time;

#ifdef _DEBUG
	return;
#endif
#ifdef DEBUG
	return;
#endif
	g_log_time = true;
	test_run_single(test_name, test_func);
	g_log_time = old_log_time;
}

void test_run_bench_v(const char* test_name, pf_test_case_v test_func, void* target, void* context) {
	bool old_log_time = g_log_time;

#ifdef _DEBUG
	return;
#endif
#ifdef DEBUG
	return;
#endif
	g_log_time = true;
	test_run_single_v(test_name, test_func, target, context);
	g_log_time = old_log_time;
}
void test_run_feature_v(const char* test_name, pf_test_case_v test_func, void* target, void* context);


void test_run_feature(const char* test_name, pf_test_case test_func) {
	test_run_single(test_name, test_func);
}

void test_log_use_file(const char* file_name) {
	g_log_file = file_name;
}

void test_log_use_header(bool use_or_not) {
	g_log_header = use_or_not;
}

void test_log_use_footer(bool use_or_not) {
	g_log_footer = use_or_not;
}

void test_log_time(bool log) {
	g_log_time = log;
}

void allocator_heap_walk_print(struct heap_blockinfo* binfo, void* param) {
	unused(param);
	if (binfo->allocated) {
		log_printtab();
		log_printf("0X%08X", (int)(intptr_t)binfo->allocable_addr);
		log_printf("%8d bytes", binfo->allocable_size);
		log_printf(" at %s %d\n", binfo->file, binfo->line);
	}
}
