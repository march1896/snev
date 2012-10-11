#include <heap_llrb.h>
#include <heap_buddy.h>
#include <test_util.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define N 5000000
static void *pointers[N];
static void *buff;
static heap_handle ph;
enum __HEAP_TYPE {
	HEAP_SYSTEM,
	HEAP_LLRB,
	HEAP_BUDDY
} heap_type;

const char *HEAP_NAMES[] = {
	"System",
	"LLRB",
	"BUDDY"
};

static void _init_heap() {
	if (heap_type == HEAP_SYSTEM)
		return;

	buff = malloc(1 << 30);

	if (heap_type == HEAP_LLRB)
		ph = theap_init(buff, 1 << 30);
	else if (heap_type == HEAP_BUDDY)
		ph = bheap_init(buff, 1 << 30);
}

static void _deinit_heap() {
	if (heap_type == HEAP_SYSTEM)
		return;

	if (heap_type == HEAP_LLRB)
		theap_destroy(ph);
	else if (heap_type == HEAP_BUDDY)
		bheap_destroy(ph);

	free(buff);
}

static void _allocate_data(int n) {
	int i;
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

	for (i = 0; i < n; i ++) {
		int sz = rand() % (10 * 1024);
		if (heap_type == HEAP_SYSTEM)
			pointers[i] = malloc(sz);
		else if (heap_type == HEAP_LLRB) {
#ifdef _MEM_DEBUG_
			pointers[i] = theap_alloc_debug(ph, sz, __FILE__, __LINE__);
#else
			pointers[i] = theap_alloc(ph, sz);
#endif
		}
		else if (heap_type == HEAP_BUDDY) {
#ifdef _MEM_DEBUG_
			pointers[i] = bheap_alloc_debug(ph, sz, __FILE__, __LINE__);
#else 
			pointers[i] = bheap_alloc(ph, sz);
#endif
		}
	}
}

static void _dealloc_data(int n) {
	int i;

	for (i = 0; i < n-1; i ++) {
		if (heap_type == HEAP_SYSTEM)
			free(pointers[i]);
		else if (heap_type == HEAP_LLRB)
			theap_dealloc(ph, pointers[i]);
		else if (heap_type == HEAP_BUDDY)
			bheap_dealloc(ph, pointers[i]);
	}
}

static void debug_leak() {
#ifdef _MEM_DEBUG_
	if (heap_type == HEAP_SYSTEM)
		return;
	else if (heap_type == HEAP_LLRB)
		theap_debug_leak(ph);
	else if (heap_type == HEAP_BUDDY)
		bheap_debug_leak(ph);
#endif
}

void heap_correctness_sub() {
	int length = 1000;
	printf("%s heap correctness test begin\n", HEAP_NAMES[heap_type]);

	_init_heap();
	_allocate_data(length);
	_dealloc_data(length);

	debug_leak();

	_deinit_heap();
	printf("%s heap correctness test end\n", HEAP_NAMES[heap_type]);
}

void heap_correctness_test() {
	heap_type = HEAP_LLRB;
	heap_correctness_sub();

	heap_type = HEAP_BUDDY;
	heap_correctness_sub();
}

void heap_performance_sub() {
	clock_t start_c, end_c;
	int length = N;
	int count;

	printf("%s heap performance test begin\n", HEAP_NAMES[heap_type]);
	_init_heap();

	memset(pointers, 0x0, sizeof(pointers));
	start_c = clock();	

	for (count = 0; count < N; count ++) {
		int idx = (rand() % N + N) % N;
		if (pointers[idx] == NULL) {
			int sz = rand() % (10 * 1024);
			if (heap_type == HEAP_SYSTEM)
				pointers[idx] = malloc(sz);
			else if (heap_type == HEAP_LLRB)
				pointers[idx] = theap_alloc(ph, sz);
			else if (heap_type == HEAP_BUDDY)
				pointers[idx] = bheap_alloc(ph, sz);
		}
		else {
			if (heap_type == HEAP_SYSTEM)
				free(pointers[idx]);
			else if (heap_type == HEAP_LLRB)
				theap_dealloc(ph, pointers[idx]);
			else if (heap_type == HEAP_BUDDY)
				bheap_dealloc(ph, pointers[idx]);

			pointers[idx] = NULL;
		}
	}

	end_c = clock();
	printf("allocate/deallocate %d elements used %f\n", length, (float)(end_c - start_c)/CLOCKS_PER_SEC);

	_deinit_heap();

	printf("%s heap performance test end\n", HEAP_NAMES[heap_type]);
}

void heap_performance_test() {
	heap_type = HEAP_SYSTEM;
	heap_performance_sub();

	heap_type = HEAP_LLRB;
	heap_performance_sub();

	heap_type = HEAP_BUDDY;
	heap_performance_sub();
}

void heap_test() {
	heap_correctness_test();
	heap_performance_test();
}
