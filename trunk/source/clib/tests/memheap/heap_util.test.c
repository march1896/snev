#include <heap_util.test.h>

/* alloc/dealloc operation order */
enum heaptest_op_order {
	heap_op_increasing, /* first alloc all data, then dealloc them in the same order */
	heap_op_decreasing, /* first alloc all data, then dealloc them in reverse order */
	heap_op_alternately, /* alloc dealloc alternately */
	heap_op_random,     /* totally random order */
	heap_op_patterned0, 
	heap_op_patterned1,
	heap_op_patterned2,

	heap_op_order_count
};

enum heaptest_page_size {
	heap_pagesize_small,
	heap_pagesize_middle,
	heap_pagesize_large,

	heap_pagesize_size_count
};

enum heaptest_page_pattern {
	heap_pagepattern_unique,     /* all pages are the same */
	heap_pagepattern_increasing, /* page size is increasing */
	heap_pagepattern_decreasing, /* page size is decreasing */
	heap_pagepattern_jag,        /* page size is jagged */
	heap_pagepattern_random_patterned, /* random page size, but size is pattern, in other words, not too much different king of size */
	heap_pagepattern_random,     /* totally random page size */
	heap_pagepattern_special0,

	heap_pagepattern_pattern_count
};


static void*      g_heap;
static pf_alloc   g_alloc;
static pf_dealloc g_dealloc;

static bool       g_fill_data;
static bool       g_log_time;

static enum heaptest_op_order     g_op_order;
static enum heaptest_page_size    g_page_size;
static enum heaptest_page_pattern g_page_pattern;

static int   g_last_gen_size;

static int**      g_pagesize;
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
}

/* the return value depends on pagesize and pagepattern */
static int generate_pagesize() {

}

static void generate_begin() {
}

static void generate_end() {
}

void f_heap_op_increasing() {
	int i;

	dbg_assert(g_pagesize_length == g_address_length);

	/* first generate test data */
	generate_begin();
	for (i = 0; i < g_pagesize_length; i ++) {
		g_pagesize[i] = generate();
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
				g_address[i][j] = NULL;
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

void heaptest_set_heap(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc) {
	g_heap     = __heap;
	g_alloc    = __alloc;
	g_dealloc  = __dealloc;
}

void heaptest_set_debugstate(bool filldata, bool logtime) {
}

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

void heaptest_fun_allcomb(int __length) {
	enum heaptest_op_order order;
	enum heaptest_page_size pagesize
	enum heaptest_page_pattern pagepattern;

	for (order = heap_op_increasing; order < heap_op_order_count; order ++) {
		for (pagesize = heap_pagesize_small; pagesize < heap_pagesize_count; pagesize ++) {
			for (pagepattern = heap_pagepattern_unique; pagepattern < heap_pagepattern_count; pagepattern ++) {
				heaptest_run_single(__length, order, pagesize, pagepattern);
			}
		}
	}
}
