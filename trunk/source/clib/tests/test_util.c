#include <test_util.h>

#include <stdio.h>
#include <stdarg.h>
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

void move_tab(bool inc) {
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
	va_start( arglist, format );
	vprintf(format, arglist);
	if (g_use_file) {
		FILE * pFile = fopen(g_log_file, "a");
		vfprintf(pFile, format, arglist);
		fclose(pFile);
	}
	va_end( arglist );
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

	va_start( arglist, format );
	printf("%s", g_table_buff);
	vprintf(format, arglist);
	printf("\n");
	if (g_use_file) {
		FILE * pFile = fopen(g_log_file, "a");
		fprintf(pFile, "%s", g_table_buff);
		vfprintf(pFile, format, arglist);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
	va_end( arglist );
}

void test_run_single(const char* test_name, pf_test_case test_func) {
	clock_t start_c, end_c;
	start_c = clock();

	if (test_name == NULL) {
		test_name = "unknown test";
	}

	move_tab(true);

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

	move_tab(false);
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
