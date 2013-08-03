#include <test_util.h>

#include <stdio.h>
#include <time.h>

static bool        g_use_file = false;
static const char* g_log_file = NULL;
static bool        g_log_header = true;
static bool        g_log_footer = true;
static bool        g_log_time = true;

void test_run_single(const char* test_name, pf_test_case test_func) {
	clock_t start_c, end_c;
	start_c = clock();

	if (test_name == NULL) {
		test_name = "unknown test";
	}

	if (g_log_header) {
		printf("[ %s begins ]\n", test_name);
		if (g_use_file) {
			FILE * pFile = fopen(g_log_file, "a");
			fprintf(pFile, "[ %s begins ]\n", test_name);
			fclose(pFile);
		}
	}

	test_func();

	end_c = clock();
	if (g_log_time) {
		printf("[ %s total time use %f ]\n", test_name, (float)(end_c - start_c)/CLOCKS_PER_SEC);
		if (g_use_file) {
			FILE * pFile = fopen(g_log_file, "a");
			fprintf(pFile, "[ %s total time use %f ]\n", test_name, (float)(end_c - start_c)/CLOCKS_PER_SEC);
			fclose(pFile);
		}
	}

	if (g_log_footer) {
		printf("[ %s ends ]\n", test_name);
		if (g_use_file) {
			FILE * pFile = fopen(g_log_file, "a");
			fprintf(pFile, "[ %s ends ]\n", test_name);
			fclose(pFile);
		}
	}
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
