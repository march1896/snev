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
		if (rawdata[i] != z[i]) {
			printf("error!\n");
			break;
		}
}

static void assign_z(citer itr) {
	z[i++] = (int)citer_get_ref(itr);
}

typedef void (*pf_sort_func)(citer begin, citer end, pf_compare_object comp);

static void sort_correctness_test(const char* sort_name, pf_sort_func the_sort, 
		test_cont_type ct,
		test_data_type data_type, 
		test_data_length data_length
		) 
{
	citer_dos(begin, NULL);
	citer_dos(end, NULL);
	int length, ulength;

	printf("%s sorting on %s with order %s \n", sort_name, cntr_name(ct), data_order_name(data_type));
	generate_test_data(data_type, data_length, &length, &ulength);

	c = cntr_create(ct);

	for (i = 0; i < length; i ++) cntr_add_back(c, (void*)rawdata[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	
	the_sort(begin, end, cntr_int_compare);
	i = 0;
	citer_for_each(begin, end, assign_z);
	qsort(rawdata, length, sizeof(int), qsort_int_compare);

	judge(length);

	cntr_destroy(c);
}

static void sort_performance_test(char* sort_name, pf_sort_func the_sort,
		test_cont_type ct,
		test_data_type data_type, 
		test_data_length data_length
		) 
{
	clock_t start_c, end_c;
	int length, ulength;
	citer_dos(begin, NULL);
	citer_dos(end, NULL);

	generate_test_data(data_type, data_length, &length, &ulength);
	c = cntr_create(ct);
	for (i = 0; i < length; i ++) cntr_add_back(c, (void*)rawdata[i]);

	cntr_citer_begin(c, begin);
	cntr_citer_end(c, end);
	start_c = clock();	
	if (strcmp(sort_name, "default") == 0){
		qsort(rawdata, length, sizeof(int), qsort_int_compare);
	}
	else {
		the_sort(begin, end, cntr_int_compare);
	}
	end_c = clock();
	printf("%s sorting on %s with order %s used %f\n", sort_name, cntr_name(ct), data_order_name(data_type), (float)(end_c - start_c)/CLOCKS_PER_SEC);
	cntr_destroy(c);
}

char c_sort[][30] = { "bubble", "quick", "merge", "default"};
pf_sort_func f_sort[] = { bubble_sort, quick_sort, merge_sort, bubble_sort };
char c_order[][30] = { "increase", "decrease", "random" };

void cntr_sort_test() {
	int i, j, k;

	printf("correctness test start\n");
	for (i = 0; i < 3; i ++) {
		for (j = 0; j < 2; j ++) {
			for (k = 0; k < 3; k ++) {
				sort_correctness_test(c_sort[i], f_sort[i], (test_cont_type)j, (test_data_type)k, el_correctness);
			}
		}
	}
	printf("correctness test end\n\n");

	printf("performance test start\n");
	for (i = 0; i < 4; i ++) {
		for (j = ec_list; j <= ec_array; j ++) {
			for (k = ed_increase; k < ed_end; k ++) {
				sort_performance_test(c_sort[i], f_sort[i], (test_cont_type)j, (test_data_type)k, el_performance);
			}
		}
	}
	printf("performance test end\n\n");
	
}

