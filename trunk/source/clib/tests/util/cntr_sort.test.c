#include <cntr.h>
#include <test_util.h>

#include <stdio.h>
#include <stdlib.h>

static void print(citer itr) { printf("%d ", *((int*)citer_get_ref(itr))); }

int int_addr_compare(const void* x, const void* y) {
	int a = *((int*)x);
	int b = *((int*)y);

	if (a < b) return -1;
	else if (a > b) return 1;

	return 0;
}

int int_compare(const void* x, const void* y) {
	int a = (int)x;
	int b = (int)y;

	if (a < b) return -1;
	else if (a > b) return 1;

	return 0;
}

int qsort_compare(const void* x, const void* y) {
	int* a = (int*)x;
	int* b = (int*)y;

	if (*a < *b) return -1;
	else if (*a > *b) return 1;

	return 0;
}

#define CLENGTH 10000

static int x[CLENGTH], y[CLENGTH], z[CLENGTH], i;
static cntr c;

static void init_data_increase(int length) {
	for (i = 0; i < length; i ++) {
		x[i] = y[i] = i;
		z[i] = y[i];
	}
}

static void init_data_decrease(int length) {
	for (i = 0; i < length; i ++) {
		x[i] = y[i] = length - i;
		z[i] = y[i];
	}
}

static void init_data_random(int length) {
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);
	for (i = 0; i < length; i ++) {
		x[i] = y[i] = rand();
		z[i] = y[i];
	}
}

static void judge(int length) {
	for (i = 0; i < length; i ++) 
		if (x[i] != y[i]) {
			printf("error!\n");
			break;
		}
}

static void assign_x(citer itr) {
	x[i++] = (int)citer_get_ref(itr);
}

typedef void (*pf_init_data)(int);
typedef void (*pf_sort_name)(citer begin, citer end, pf_compare_object comp);
typedef cntr (*pf_cntr_create)();
static void sort_correctness_test(char* sort_name, char* cntr_name, char* data_order, 
		pf_sort_name the_sort,
		pf_cntr_create cntr_create,
		pf_init_data init_data, 
		int data_length
		) 
{
	citer_dos(begin, NULL);
	citer_dos(end, NULL);
	printf("%s sorting on %s with order %s \n", sort_name, cntr_name, data_order);

	init_data(data_length);

	c = cntr_create();

	for (i = 0; i < data_length; i ++) cntr_add_back(c, (void*)x[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	
	the_sort(begin, end, int_compare);
	i = 0;
	citer_for_each(begin, end, assign_x);
	qsort(y, data_length, sizeof(int), qsort_compare);

	judge(data_length);

	cntr_destroy(c);
}

static void sort_performance_test(char* sort_name, char* cntr_name, char* data_order, 
		pf_sort_name the_sort,
		pf_cntr_create cntr_create,
		pf_init_data init_data, 
		int data_length
		) 
{
	clock_t start_c, end_c;
	citer_dos(begin, NULL);
	citer_dos(end, NULL);

	init_data(data_length);
	c = cntr_create();
	for (i = 0; i < data_length; i ++) cntr_add_back(c, (void*)x[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	start_c = clock();	
	the_sort(begin, end, int_compare);
	end_c = clock();
	printf("%s sorting on %s with order %s used %f\n", sort_name, cntr_name, data_order, (float)(end_c - start_c)/CLOCKS_PER_SEC);
	cntr_destroy(c);
}

char c_sort[][30] = { "bubble", "quick", "merge" };
pf_sort_name f_sort[] = { bubble_sort, quick_sort, merge_sort };
char c_cntr[][30] = { "list", "array" };
pf_cntr_create f_create[] = { cntr_create_as_list, cntr_create_as_array };
char c_order[][30] = { "increase", "decrease", "random" };
pf_init_data f_order[] = { init_data_increase, init_data_decrease, init_data_random };

void cntr_sort_test() {
	int i, j, k;

	printf("correctness test start\n");
	for (i = 0; i < 3; i ++) {
		for (j = 0; j < 2; j ++) {
			for (k = 0; k < 3; k ++) {
				sort_correctness_test(c_sort[i], c_cntr[j], c_order[k], f_sort[i], f_create[j], f_order[k], 100);
			}
		}
	}
	printf("correctness test end\n\n");

	printf("performance test start\n");
	for (i = 0; i < 3; i ++) {
		for (j = 0; j < 2; j ++) {
			for (k = 0; k < 3; k ++) {
				sort_performance_test(c_sort[i], c_cntr[j], c_order[k], f_sort[i], f_create[j], f_order[k], CLENGTH);
			}
		}
	}
	printf("performance test end\n\n");
	
}

