#include <cntr.h>

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

static void init_data_increase() {
	for (i = 0; i < CLENGTH; i ++) {
		x[i] = y[i] = i;
		z[i] = y[i];
	}
}

static void init_data_decrease() {
	for (i = 0; i < CLENGTH; i ++) {
		x[i] = y[i] = CLENGTH - i;
		z[i] = y[i];
	}
}

static void init_data_random() {
	for (i = 0; i < CLENGTH; i ++) {
		x[i] = y[i] = ((i*73+17)%23+11);
		z[i] = y[i];
	}
}

static void init_list_with_data() {
	c = cntr_create_as_list();

	for (i = 0; i < CLENGTH; i ++) cntr_add_back(c, &x[i]);
}

static void judge() {
	for (i = 0; i < CLENGTH; i ++) 
		if (x[i] != y[i]) {
			printf("error!");
			break;
		}
}

static void assign_x(citer itr) {
	x[i++] = (int)citer_get_ref(itr);
}

typedef void (*pf_init_data)();
typedef void (*pf_sort_name)(citer begin, citer end, pf_compare_object comp);
typedef cntr (*pf_cntr_create)();
static void test_sort_case(char* sort_name, char* cntr_name, char* data_order, 
		pf_sort_name the_sort,
		pf_cntr_create cntr_create,
		pf_init_data init_data 
		) 
{
	citer_dos(begin, NULL);
	citer_dos(end, NULL);
	printf("%s sorting on %s with order %s start\n", sort_name, cntr_name, data_order);

	init_data();

	c = cntr_create();

	for (i = 0; i < CLENGTH; i ++) cntr_add_back(c, (void*)x[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	the_sort(begin, end, int_compare);
	i = 0;
	citer_for_each(begin, end, assign_x);
	qsort(y, CLENGTH, sizeof(int), qsort_compare);

	judge();

	cntr_destroy(c);
}

char c_sort[][30] = { "bubble", "quick" };
pf_sort_name f_sort[] = { bubble_sort, quick_sort };
char c_cntr[][30] = { "list", "array" };
pf_cntr_create f_create[] = { cntr_create_as_list, cntr_create_as_array };
char c_order[][30] = { "increase", "decrease", "random" };
pf_init_data f_order[] = { init_data_increase, init_data_decrease, init_data_random };

void cntr_sort_test() {
	int i, j, k;
	for (i = 0; i < 2; i ++) {
		for (j = 0; j < 2; j ++) {
			for (k = 0; k < 3; k ++) {
				test_sort_case(c_sort[i], c_cntr[j], c_order[k], f_sort[i], f_create[j], f_order[k]);
			}
		}
	}
}

