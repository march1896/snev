#include <memheap/heap_util.test.h>

#include <memheap/heap_def.h>

#include <time.h>
#include <stdlib.h>

static void*      g_heap;
static pf_alloc   g_alloc;
static pf_dealloc g_dealloc;

/* global state to control the output */
static bool       g_fill_data;
static bool       g_log_operation;
static const char* g_log_operation_file;
static bool       g_log_time;
static const char* g_log_time_file;
static int        g_log_time_format;

/* global state of the current test */
/* total number of heap operation */
static int        g_op_count;
/* continuous heap operation length */
static int        g_op_step_length;
static enum ht_op_order     g_op_order;
static enum ht_data_size    g_page_size;
static enum ht_data_pattern g_page_pattern;

static int        g_heap_op_alter_range_min;
static int        g_heap_op_alter_range_max;
static int        g_pagepattern_jag_range_min;
static int        g_pagepattern_jag_range_max;
static int        g_pagepattern_random_restricted_count;

static int*       g_seeds_inuse;

static int        g_preload_do;
static int        g_preload_alloc_count;
static int        g_preload_dealloc_count;
static void**     g_preload_address;
static void       heap_load_preload();
static void       heap_release_preload();

static int        g_pagesize_min = -1;
static int        g_pagesize_max = -1;

static const int  SMALL_PAGE_MIN  = 1;
static const int  SMALL_PAGE_MAX  = HEAP_MINIMUM_EXPAND_SIZE/20;
static const int  MIDDLE_PAGE_MIN = HEAP_MINIMUM_EXPAND_SIZE/20;
static const int  MIDDLE_PAGE_MAX = HEAP_MINIMUM_EXPAND_SIZE/4;
static const int  LARGE_PAGE_MIN  = HEAP_MINIMUM_EXPAND_SIZE/4;
static const int  LARGE_PAGE_MAX  = HEAP_MINIMUM_EXPAND_SIZE;
static const int  HUGE_PAGE_MIN   = HEAP_MINIMUM_EXPAND_SIZE;
static const int  HUGE_PAGE_MAX   = HEAP_MINIMUM_EXPAND_SIZE*5;
static const int  RANDOM_PAGE_MIN = 1;
static const int  RANDOM_PAGE_MAX = HEAP_MINIMUM_EXPAND_SIZE*5;

const char* ht_op_name[] = {
	"op_inorder",
	"op_reverseorder",
	"op_alternately",
	"op_random",
	"op_patterned0", 
	"op_patterned1",
	"op_patterned2",
	"op_order_count"
};

const char* ht_size_name[] = {
	"data_small",
	"data_middle",
	"data_large",
	"data_huge",
	"data_random",
	"data_count"
};

const char* ht_pattern_name[] = {
	"pattern_unique",
	"pattern_increasing",
	"pattern_decreasing",
	"pattern_jag",
	"pattern_random_restricted",
	"pattern_random",
	"pattern_special0",
	"pattern_special1",
	"pattern_special2",
	"pattern_count"
};

typedef void (*pf_heap_op_order)();
void f_heap_op_inorder();
void f_heap_op_reverseorder();
void f_heap_op_alternately();
void f_heap_op_random();

static pf_heap_op_order heap_op_callbacks[] = {
	f_heap_op_inorder,
	f_heap_op_reverseorder,
	f_heap_op_alternately,
	f_heap_op_random,
};

static void generate_init_current() {
	switch (g_page_size) {
		case ht_data_small:
			g_pagesize_min = SMALL_PAGE_MIN;
			g_pagesize_max = SMALL_PAGE_MAX;
			break;
		case ht_data_middle:
			g_pagesize_min = MIDDLE_PAGE_MIN;
			g_pagesize_max = MIDDLE_PAGE_MAX;
			break;
		case ht_data_large:
			g_pagesize_min = LARGE_PAGE_MIN;
			g_pagesize_max = LARGE_PAGE_MAX;
			break;
		case ht_data_huge:
			g_pagesize_min = HUGE_PAGE_MIN;
			g_pagesize_max = HUGE_PAGE_MAX;
			break;
		case ht_data_random:
			g_pagesize_min = RANDOM_PAGE_MIN;
			g_pagesize_max = RANDOM_PAGE_MAX;
			break;
	}
}

static int generate_in_range(int minimum, int maximum) {
	if (minimum >= maximum) return minimum;

	dbg_assert(maximum - minimum > 0);
	return rand() % (maximum - minimum) + minimum;
}

static void heap_load_preload() {
	int i;

	g_preload_address = (void**)malloc(sizeof(void*) * g_preload_alloc_count);

	for (i = 0; i < g_preload_alloc_count; i ++) {
		int size = generate_in_range(g_pagesize_min, g_pagesize_max);

		g_preload_address[i] = (void*)alloc(g_alloc, g_heap, size);
	}

	for (i = 0; i < g_preload_dealloc_count; i ++) {
		int index = rand() % g_preload_alloc_count;

		if (g_preload_address[index] != NULL) {

			dealloc(g_dealloc, g_heap, g_preload_address[index]);

			g_preload_address[index] = NULL;
		}
	}
}

static void heap_release_preload() {
	int i;

	/* in fact, we only need to release the preload for system heap */
	for (i = 0; i < g_preload_alloc_count; i ++) {
		if (g_preload_address[i] != NULL) {
			dealloc(g_dealloc, g_heap, g_preload_address[i]);

			g_preload_address[i] = NULL;
		}
	}
}

static int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}
static void data_seeds_init() {
	int i;
	int* seeds_random  = (int*)malloc(sizeof(int) * g_op_count);
	int* seeds_inorder = (int*)malloc(sizeof(int) * g_op_count);

	g_seeds_inuse   = (int*)malloc(sizeof(int) * g_op_count);

	for (i = 0; i < g_op_count; i ++) {
		seeds_random[i] = generate_in_range(g_pagesize_min, g_pagesize_max);
		seeds_inorder[i] = seeds_random[i];
	}

	qsort(seeds_inorder, g_op_count, sizeof(int), cmpfunc);

	switch (g_page_pattern) {
		int index;
		case ht_pattern_unique:
			index = generate_in_range(0, g_op_count);
			for (i = 0; i < g_op_count; i ++) {
				g_seeds_inuse[i] = seeds_random[index];
			}
			break;
		case ht_pattern_increasing:
			for (i = 0; i < g_op_count; i ++) {
				g_seeds_inuse[i] = seeds_inorder[i];
			}
			break;
		case ht_pattern_decreasing:
			for (i = 0; i < g_op_count; i ++) {
				g_seeds_inuse[i] = seeds_inorder[g_op_count-1-i];
			}
			break;
		case ht_pattern_jag:
			/* TODO: jag is not implemented, currently using random */
			for (i = 0; i < g_op_count; i ++) {
				g_seeds_inuse[i] = seeds_random[i];
			}
		case ht_pattern_random_restricted:
			dbg_assert(g_pagepattern_random_restricted_count < g_op_count);

			for (i = 0; i < g_op_count; i ++) {
				int index = generate_in_range(0, g_pagepattern_random_restricted_count);
				g_seeds_inuse[i] = seeds_inorder[index];
			}
		case ht_pattern_random:
			for (i = 0; i < g_op_count; i ++) {
				g_seeds_inuse[i] = seeds_random[i];
			}
			break;
	}

	free(seeds_random);
	free(seeds_inorder);
}

static void data_seeds_deinit() {
	free(g_seeds_inuse);
}

static int get_alloc_size(int ith_op) {
	return g_seeds_inuse[ ith_op / g_op_step_length ];
}

void f_heap_op_inorder() {
	int i;
	char** addresses = (char**)malloc(sizeof(char*) * g_op_count);
	/* allocate test data in order */
	for (i = 0; i < g_op_count; i ++) {
		addresses[i] = (char*)alloc(g_alloc, g_heap, get_alloc_size(i));
	}

	if (g_fill_data) {
		for (i = 0; i < g_op_count; i ++) {
			int size = get_alloc_size(i);
			int j;
			for (j = 0; j < size; j ++) {
				addresses[i][j] = 0;
			}
		}
	}

	/* last, dealloc data in order */
	for (i = 0; i < g_op_count; i ++) {
		dealloc(g_dealloc, g_heap, addresses[i]);
	}

	free(addresses);
}

void f_heap_op_reverseorder() {
	int i;
	char** addresses = (char**)malloc(sizeof(char*) * g_op_count);
	/* allocate test data in order */
	for (i = 0; i < g_op_count; i ++) {
		addresses[i] = (char*)alloc(g_alloc, g_heap, get_alloc_size(i));
	}

	if (g_fill_data) {
		for (i = 0; i < g_op_count; i ++) {
			int size = get_alloc_size(i);
			int j;
			for (j = 0; j < size; j ++) {
				((char*)addresses[i])[j] = 0;
			}
		}
	}

	/* last, dealloc data in reverse order */
	for (i = g_op_count-1; i >= 0; i --) {
		dealloc(g_dealloc, g_heap, addresses[i]);
	}

	free(addresses);
}

void f_heap_op_alternately() {
	int i;
	int step_length = -1, step = 0; 
	int freelist_head = 0;
	int alloclist_head = -1;
	int op = 1; /* 1 means alloc, -1 means dealloc */
	int pool_size = 5 * g_heap_op_alter_range_max;

	/* the algorithm is a little complicated, but it ensures performance */
	char** address = (char**)malloc(sizeof(char*) * pool_size);
	int* freelist  = (int*)malloc(sizeof(int) * pool_size);
	int* alloclist = (int*)malloc(sizeof(int) * pool_size);

	/* form a memory pool to fast find an allocable unit */
	for (i = 0; i < pool_size; i ++) {
		address[i] = NULL;
		freelist[i] = i + 1;
		alloclist[i] = -1;
	}
	freelist[pool_size-1] = -1;

	step_length = generate_in_range(g_heap_op_alter_range_min, 
			g_heap_op_alter_range_max);

	for (i = 0; i < g_op_count; i ++) {
		if (op == 1) {
			/* op == 1, we should alloc */
			if (freelist_head == -1) {
				/* no allocable address, just do nothing */
			}
			else {
				dbg_assert(address[freelist_head] == NULL);
				address[freelist_head] = (char*)alloc(g_alloc, g_heap, get_alloc_size(i));
				/* add into allocated list */
				alloclist[freelist_head] = alloclist_head;
				alloclist_head = freelist_head;
				
				/* remove from freelist */
				freelist_head = freelist[freelist_head];
			}
		}
		else {
			dbg_assert(op == -1);
			/* op == -1, we should dealloc, dealloc the first address in the alloc list */
			if (alloclist_head == -1) {
				/* all are freed, do nothing */
			}
			else {
				dbg_assert(address[alloclist_head] != NULL);
				dealloc(g_dealloc, g_heap, address[alloclist_head]);
				address[alloclist_head] = NULL;

				/* add into free list */
				freelist[alloclist_head] = freelist_head;
				freelist_head = alloclist_head;

				/* remove from alloclist */
				alloclist_head = alloclist[alloclist_head];
			}
		}

		step ++;
		if (step >= step_length) {
			step_length = generate_in_range(g_heap_op_alter_range_min, 
					g_heap_op_alter_range_max);
			step = 0;

			/* we have done enough times of one kind operation, invert */
			op = -op;
		}
	}

	/* dealloc all allocated if they are not deleted */
	for (i = 0; i < pool_size; i ++) {
		if (address[i] != NULL) {
			dealloc(g_dealloc, g_heap, address[i]);
			address[i] = NULL;
		}
	}

	free(address);
	free(freelist);
	free(alloclist);
}

void f_heap_op_random() {
	int i;
	int freelist_head = 0;
	int alloclist_head = -1;
	int op = 1; /* 1 means alloc, 0 means dealloc */
	int pool_size = 5 * g_heap_op_alter_range_max;

	/* the algorithm is a little complicated, but it ensures performance */
	char** address = (char**)malloc(sizeof(char*) * pool_size);
	int* freelist  = (int*)malloc(sizeof(int) * pool_size);
	int* alloclist = (int*)malloc(sizeof(int) * pool_size);

	/* form a memory pool to fast find an allocable unit */
	for (i = 0; i < pool_size; i ++) {
		address[i] = NULL;
		freelist[i] = i + 1;
		alloclist[i] = -1;
	}
	freelist[pool_size-1] = -1;

	for (i = 0; i < g_op_count; i ++) {
		op = rand() % 2;
		if (op == 1) {
			/* op == 1, we should alloc */
			if (freelist_head == -1) {
				/* no allocable address, just do nothing */
			}
			else {
				dbg_assert(address[freelist_head] == NULL);
				address[freelist_head] = (char*)alloc(g_alloc, g_heap, get_alloc_size(i));
				/* add into allocated list */
				alloclist[freelist_head] = alloclist_head;
				alloclist_head = freelist_head;
				
				/* remove from freelist */
				freelist_head = freelist[freelist_head];
			}
		}
		else {
			/* op == -1, we should dealloc, dealloc the first address in the alloc list */
			if (alloclist_head == -1) {
				/* all are freed, do nothing */
			}
			else {
				dbg_assert(address[alloclist_head] != NULL);
				dealloc(g_dealloc, g_heap, address[alloclist_head]);
				address[alloclist_head] = NULL;

				/* add into free list */
				freelist[alloclist_head] = freelist_head;
				freelist_head = alloclist_head;

				/* remove from alloclist */
				alloclist_head = alloclist[alloclist_head];
			}
		}
	}

	/* dealloc all allocated if they are not deleted */
	for (i = 0; i < pool_size; i ++) {
		if (address[i] != NULL) {
			dealloc(g_dealloc, g_heap, address[i]);
			address[i] = NULL;
		}
	}

	free(address);
	free(freelist);
	free(alloclist);
}

void heaptest_global_state_reset() {
	g_heap                  = NULL;
	g_alloc                 = NULL;
	g_dealloc               = NULL;

	g_fill_data             = false;
	g_log_operation         = false;
	g_log_operation_file    = NULL;
	g_log_time              = false;
	g_log_time_file         = NULL;
	g_log_time_format       = 0;

	g_op_count              = -1;
	g_op_step_length        = 1;
	g_op_order              = ht_op_random;
	g_page_size             = ht_data_small;
	g_page_pattern          = ht_pattern_random_restricted;

	g_heap_op_alter_range_min             = 2;
	g_heap_op_alter_range_max             = 10;
	g_pagepattern_jag_range_min           = 3;
	g_pagepattern_jag_range_max           = 6;
	g_pagepattern_random_restricted_count = 50;

	g_seeds_inuse           = NULL;
	g_preload_do            = false;
	g_preload_alloc_count   = 1000;
	g_preload_dealloc_count = 1000;
	g_preload_address       = NULL;

	g_pagesize_min          = SMALL_PAGE_MIN;
	g_pagesize_max          = SMALL_PAGE_MAX;
}

void heaptest_begin() {
	heaptest_global_state_reset();
}

void heaptest_end() {
	heaptest_global_state_reset();
}

void heaptest_set_heap(void* __heap, pf_alloc __alloc, pf_dealloc __dealloc) {
	g_heap = __heap;
	g_alloc = __alloc;
	g_dealloc = __dealloc;
}

/* load the heap with some data before test */
void heaptest_set_preload(int alloc_count, int dealloc_count) {
	g_preload_do            = true;
	g_preload_alloc_count   = alloc_count;
	g_preload_dealloc_count = dealloc_count;
}

void heaptest_set_filldata(bool filldata) {
	g_fill_data             = filldata;
}

void heaptest_set_logoperation(bool log, const char* file) {
	g_log_operation         = log;
	g_log_operation_file    = file;
}

void heaptest_set_logtime(bool log, int format, const char* file) {
	/* format 0 is simplified, 1 is verbose */
	g_log_time              = log;
	g_log_time_file         = file;
	g_log_time_format       = format;
}

#include <stdio.h>
void heaptest_run_internal() {
	clock_t start_c, end_c;

	data_seeds_init();

	start_c = clock();	
	// call the test function
	heap_op_callbacks[g_op_order]();

	end_c = clock();
	if (g_log_time) {
		FILE * pFile = fopen(g_log_time_file, "a");
		
		fprintf(pFile, "%f    ", (float)(end_c - start_c)/CLOCKS_PER_SEC);
		fprintf(pFile, "/* %d \toperations, mode: %-15s, %-15s, %-30s*/\n", 
			g_op_count, ht_op_name[g_op_order], ht_size_name[g_page_size], ht_pattern_name[g_page_pattern]); 

		fclose(pFile);
	}

	data_seeds_deinit();
}

void heaptest_run_single(int op_count, enum ht_op_order order, enum ht_data_size pagesize, enum ht_data_pattern pagepattern) {
	g_op_count        = op_count;
	g_op_order        = order;
	g_page_size       = pagesize;
	g_page_pattern    = pagepattern;

	/* first add preload */
	if (g_preload_do) {
		heap_load_preload();
	}

	heaptest_run_internal();

	if (g_preload_do) {
		heap_release_preload();
	}
}

void heaptest_run_allcomb(int op_count) {
	enum ht_op_order order;
	enum ht_data_size pagesize;
	enum ht_data_pattern pagepattern;

	/* first add preload */
	if (g_preload_do) {
		heap_load_preload();
	}

	for (order = ht_op_inorder; order < ht_op_order_count; order ++) {
		for (pagesize = ht_data_small; pagesize < ht_data_count; pagesize ++) {
			for (pagepattern = ht_pattern_unique; pagepattern < ht_pattern_count; pagepattern ++) {
				g_op_order        = order;
				g_page_size       = pagesize;
				g_page_pattern    = pagepattern;

				g_op_count        = op_count;

				heaptest_run_internal();
			}
		}
	}

	if (g_preload_do) {
		heap_release_preload();
	}
}
