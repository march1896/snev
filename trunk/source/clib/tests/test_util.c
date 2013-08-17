#include <test_util.h>

#include <stdio.h>
#include <time.h>

static bool        g_use_file = false;
static const char* g_log_file = NULL;
static bool        g_log_header = true;
static bool        g_log_footer = true;
static bool        g_log_time = true;

static const char  g_table_char = ' ';
static char        g_table_buff[101];
static int         g_table_step = 2;
static int         g_table_length = 0;

static void move_table(bool inc) {
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

void test_run_single(const char* test_name, pf_test_case test_func) {
	clock_t start_c, end_c;
	int i;
	start_c = clock();

	if (test_name == NULL) {
		test_name = "unknown test";
	}

	move_table(true);

	if (g_log_header) {
		printf("%s[ %s begins ]\n", g_table_buff, test_name);
		if (g_use_file) {
			FILE * pFile = fopen(g_log_file, "a");
			fprintf(pFile, "%s[ %s begins ]\n", g_table_buff, test_name);
			fclose(pFile);
		}
	}

	test_func();

	end_c = clock();

	if (g_log_footer) {
		printf("%s[ %s ends", g_table_buff, test_name);
		if (g_log_time)
			printf(", total time %f ]\n", (float)(end_c - start_c)/CLOCKS_PER_SEC);
		else 
			printf(" ]\n");

		if (g_use_file) {
			FILE * pFile = fopen(g_log_file, "a");
			fprintf(pFile, "%s[ %s ends", g_table_buff, test_name);
			if (g_log_time)
				fprintf(pFile, ", total time use %f ]\n", (float)(end_c - start_c)/CLOCKS_PER_SEC);
			else 
				fprintf(pFile, " ]\n");

			fclose(pFile);
		}
	}

	move_table(false);
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
