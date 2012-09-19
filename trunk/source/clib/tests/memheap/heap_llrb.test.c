#include <heap.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define N 500000
static void *pointers[N];
static void *buff;
static heap_handle ph;

static void _init_heap() {
	buff = malloc(1 << 30);

	ph = heap_init(buff, 1 << 30);
}

static void _deinit_heap() {
	heap_destroy(ph);

	free(buff);
}

static void _allocate_data(int n) {
	int i;
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

	for (i = 0; i < n; i ++) {
		int sz = rand() % (10 * 1024);

		pointers[i] = heap_alloc(ph, sz, __FILE__, __LINE__);
	}
}

static void _dealloc_data(int n) {
	int i;

	for (i = 0; i < n; i ++) {
		heap_dealloc(ph, pointers[i], __FILE__, __LINE__);
	}
}

void heap_correctness_test() {
	int length = 2;
	printf("llrb heap correctness test begin\n");
	_init_heap();
	_allocate_data(length);
	_dealloc_data(length);
	_deinit_heap();
	printf("llrb heap correctness test end\n");
}

void heap_performance_test() {
	clock_t start_c, end_c;
	int length = N;

	printf("llrb heap performance test begin\n");
	_init_heap();

	start_c = clock();	
	end_c = clock();
	printf("allocate/deallocate %d elements used %f\n", length, (float)(end_c - start_c)/CLOCKS_PER_SEC);

	_deinit_heap();
	
	printf("llrb heap performance test end\n");
}

void heap_test() {
	heap_correctness_test();
	heap_performance_test();
}
