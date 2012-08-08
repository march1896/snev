#include <cntr.h>
#include <test_util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int i;
int z[plength];
static cntr c;

static void judge(int length) {
	for (i = 0; i < length; i ++) 
		if (x[i] != z[i]) {
			printf("error!\n");
			break;
		}
}

static void assign_z(citer itr) {
	z[i++] = (int)citer_get_ref(itr);
}

typedef void (*pf_init_data)(int);
typedef void (*pf_sort_name)(citer begin, citer end, pf_compare_object comp);
typedef cntr (*pf_cntr_create)();
static void sort_correctness_test(char* sort_name, char* cntr_name, char* data_order, 
		pf_sort_name the_sort,
		pf_cntr_create cntr_create,
		int data_type, 
		int data_length
		) 
{
	citer_dos(begin, NULL);
	citer_dos(end, NULL);
	printf("%s sorting on %s with order %s \n", sort_name, cntr_name, data_order);

	generate_data(data_type, data_length);

	c = cntr_create();

	for (i = 0; i < data_length; i ++) cntr_add_back(c, (void*)x[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	
	the_sort(begin, end, cntr_int_compare);
	i = 0;
	citer_for_each(begin, end, assign_z);
	qsort(x, data_length, sizeof(int), qsort_int_compare);

	judge(data_length);

	cntr_destroy(c);
}

static void sort_performance_test(char* sort_name, char* cntr_name, char* data_order, 
		pf_sort_name the_sort,
		pf_cntr_create cntr_create,
		int init_data_order, 
		int data_length
		) 
{
	clock_t start_c, end_c;
	citer_dos(begin, NULL);
	citer_dos(end, NULL);

	generate_data(init_data_order, data_length);
	c = cntr_create();
	for (i = 0; i < data_length; i ++) cntr_add_back(c, (void*)x[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	start_c = clock();	
	if (strcmp(sort_name, "default") == 0){
		qsort(x, data_length, sizeof(int), qsort_int_compare);
	}
	else {
		the_sort(begin, end, cntr_int_compare);
	}
	end_c = clock();
	printf("%s sorting on %s with order %s used %f\n", sort_name, cntr_name, data_order, (float)(end_c - start_c)/CLOCKS_PER_SEC);
	cntr_destroy(c);
}

char c_sort[][30] = { "bubble", "quick", "merge", "default"};
pf_sort_name f_sort[] = { bubble_sort, quick_sort, merge_sort, bubble_sort };
char c_cntr[][30] = { "list", "array" };
pf_cntr_create f_create[] = { cntr_create_as_list, cntr_create_as_array };
char c_order[][30] = { "increase", "decrease", "random" };
int f_order[] = { 0, 2, 3 };

void cntr_sort_test() {
	int i, j, k;

	printf("correctness test start\n");
	for (i = 0; i < 3; i ++) {
		for (j = 0; j < 2; j ++) {
			for (k = 0; k < 3; k ++) {
				sort_correctness_test(c_sort[i], c_cntr[j], c_order[k], f_sort[i], f_create[j], k, clength);
			}
		}
	}
	printf("correctness test end\n\n");

	printf("performance test start\n");
	for (i = 0; i < 4; i ++) {
		for (j = 0; j < 2; j ++) {
			for (k = 0; k < 3; k ++) {
				sort_performance_test(c_sort[i], c_cntr[j], c_order[k], f_sort[i], f_create[j], k, plength);
			}
		}
	}
	printf("performance test end\n\n");
	
}

