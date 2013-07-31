#include <memheap/heap_util.test.h>

#include <time.h>
#include <stdlib.h>

static void*      g_heap;
static pf_alloc   g_alloc;
static pf_dealloc g_dealloc;

static bool       g_fill_data;
static bool       g_log_time;

static enum heaptest_op_order     g_op_order;
static enum heaptest_page_size    g_page_size;
static enum heaptest_page_pattern g_page_pattern;

static int        g_last_sizevalue;
/* next trend, < 0 means will smaller than g_last_sizevalue,
 * == 0 means will equal to g_last_sizevalue,
 * > 0 means will greater than g_last_size value.*/
static int        g_next_trend;
static int        g_step_length;

static int*       g_pagesize;
static int        g_pagesize_length;
static void**     g_address;
static int        g_address_length;

typedef void (*pf_heap_op_order)();
void f_heap_op_increasing();
void f_heap_op_decreasing();
void f_heap_op_alternately();
void f_heap_op_random();
void f_heap_op_patterned0();
void f_heap_op_patterned1();
void f_heap_op_patterned2();

static pf_heap_op_order heap_op_callbacks[] = {
	f_heap_op_increasing,
	f_heap_op_decreasing,
	f_heap_op_alternately,
	f_heap_op_random,
	f_heap_op_patterned0,
	f_heap_op_patterned1,
	f_heap_op_patterned2,
};

enum heaptest_page_size {
	heap_pagesize_small,        /* smaller than 1/20 of expand size */
	heap_pagesize_middle,       /* between 1/20 and 1/4 of expand size */
	heap_pagesize_large,        /* between 1/4 and 1 expand size */
	heap_pagesize_huge,         /* between 1 and 5 expand size */
	heap_pagesize_random,       /* totally random */
	heap_pagesize_count
};

enum heaptest_page_pattern {
	heap_pagepattern_unique,     /* all pages are the same */
	heap_pagepattern_increasing, /* page size is increasing */
	heap_pagepattern_decreasing, /* page size is decreasing */
	heap_pagepattern_jag,        /* page size is jagged */
	heap_pagepattern_random_patterned, /* random page size, but size is pattern, in other words, not too much different king of size */
	heap_pagepattern_random,     /* totally random page size */
	heap_pagepattern_special0,

	heap_pagepattern_count
};

static enum heaptest_page_size    g_page_size;
static enum heaptest_page_pattern g_page_pattern;

static int small_page_min  = 1;
static int small_page_max  = HEAP_MINIMUM_EXPAND_SIZE/20;
static int middle_page_min = small_page_max;
static int middle_page_max = HEAP_MINIMUM_EXPAND_SIZE/4;
static int large_page_min  = middle_page_max;
static int large_page_max  = HEAP_MINIMUM_EXPAND_SIZE;
static int huge_page_min   = large_page_max;
static int huge_page_max   = HEAP_MINIMUM_EXPAND_SIZE*5;
static int random_page_min = small_page_min;
static int random_page_max = huge_page_max;

static int current_page_min = -1;
static int current_page_max = -1;

static void generate_reset_current() {
	switch (g_page_size) {
		case heap_pagesize_small:
			current_page_min = small_page_min;
			current_page_max = small_page_max;
			break;
		case heap_pagesize_middle:
			current_page_min = middle_page_min;
			current_page_max = middle_page_max;
			break;
		case heap_pagesize_large:
			current_page_min = large_page_min;
			current_page_max = large_page_max;
			break;
		case heap_pagesize_huge:
			current_page_min = huge_page_min;
			current_page_max = huge_page_max;
			break;
		case heap_pagesize_random:
			current_page_min = random_page_min;
			current_page_max = random_page_max;
			break;
	}
}

static int generate_in_range(int minimum, int maximum) {
	if (minimum >= maximum) return minimum;

	dbg_assert(maximum - minimum > 0);
	return rand() % (maximum - minimum) + minimum;
}

static int generate_single_value() {
	return generate_in_range(current_page_min, current_page_max);
}

static int generate_shrink_range(int n_value) {
	dbg_assert(n_value >= current_page_min && n_value <= current_page_max);
	g_steps ++;
	if (g_steps >= g_step_legnth) {
		if (g_next_trend > 0) {
			current_page_min = n_value + 1;
		}
		else if (g_next_trend == 0)  {
			current_page_min = current_page_max = n_value;
		}
		else {
			current_page_max = n_value - 1;
		}
		g_steps = 0;
	}
}

/* the return value depends on pagesize and pagepattern */
static int generate_pagesize() {
	int gen = -1;
	switch (g_page_pattern) {
		case heap_pagepattern_unique:
		case heap_pagepattern_increasing:
		case heap_pagepattern_decreasing:
			gen = generate_single_value();
			dbg_assert(gen > 0);
			generate_shrink_range();
			return gen;
		case heap_pagepattern_jag:
			/* TODO: */
		case heap_pagepattern_random_patterned:
		case heap_pagepattern_random:
		case heap_pagepattern_special0:
	}
}

static void generate_begin() {
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

	g_last_sizevalue = -1;
	g_step_length = 0;

	int small_page_min;
	int small_page_max;
	int middle_page_min;
	int middle_page_max;
	int large_page_min;
	int large_page_max;
	int huge_page_min;
	int huge_page_max;
}

static void generate_end() {
}

void f_heap_op_increasing() {
	int i;

	dbg_assert(g_pagesize_length == g_address_length);

	/* first generate test data */
	generate_begin();
	for (i = 0; i < g_pagesize_length; i ++) {
		g_pagesize[i] = generate_pagesize();
	}
	generate_end();

	/* allocate test data in order */
	for (i = 0; i < g_pagesize_length; i ++) {
		g_address[i] = (void*)alloc(g_alloc, g_heap, g_pagesize[i]);
	}

	if (g_fill_data) {
		for (i = 0; i < g_pagesize_length; i ++) {
			int j;
			for (j = 0; j < g_pagesize[i]; j ++) {
				((char*)g_address[i])[j] = 0;
			}
		}
	}

	/* last, dealloc data in order */
	for (i = 0; i < g_pagesize_length; i ++) {
		dealloc(g_dealloc, g_heap, g_address[i]);
	}
}

void f_heap_op_decreasing() {
}
void f_heap_op_alternately() {
}
void f_heap_op_random() {
}
void f_heap_op_patterned0() {
}	
void f_heap_op_patterned1() {
}
void f_heap_op_patterned2() {
}

void heaptest_begin() {
	/* reset static global variable */
	g_heap     = NULL;
	g_alloc    = NULL;
	g_dealloc  = NULL;
}

void heaptest_end() {
	/* reset static global variable */
	g_heap     = NULL;
	g_alloc    = NULL;
	g_dealloc  = NULL;
}

void heaptest_set_heap(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc) {
	g_heap     = __heap;
	g_alloc    = __alloc;
	g_dealloc  = __dealloc;
}

void heaptest_set_filldata(bool filldata) {
}

void heaptest_set_logdata(bool log, const char* file);
/* format 0 is simplified, 1 is verbosed */
void heaptest_set_logtime(bool log, int format, const char* file);

void heaptest_run_internal() {
}

void heaptest_run_single(int __length, enum heaptest_op_order __order, enum heaptest_page_size __pagesize, enum heaptest_page_pattern __pagepattern) {
	g_pagesize_length = __length;
	g_address_length  = __length;

	g_op_order        = __order;
	g_page_size       = __pagesize;
	g_page_pattern    = __pagepattern;

	heaptest_run_internal();
}

void heaptest_run_allcomb(int __length) {
	enum heaptest_op_order order;
	enum heaptest_page_size pagesize;
	enum heaptest_page_pattern pagepattern;

	for (order = heap_op_increasing; order < heap_op_order_count; order ++) {
		for (pagesize = heap_pagesize_small; pagesize < heap_pagesize_count; pagesize ++) {
			for (pagepattern = heap_pagepattern_unique; pagepattern < heap_pagepattern_count; pagepattern ++) {
				heaptest_run_single(__length, order, pagesize, pagepattern);
			}
		}
	}
}
