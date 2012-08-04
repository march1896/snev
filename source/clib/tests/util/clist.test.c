#include <cntr_list.h>
#include <cntr_algorithm.h>
#include <cntr_iterator.h>
#include <cntr_abs.h>

#include <stdio.h>

static void print(citer itr) { printf("%d ", *((int*)citer_get_ref(itr))); }

void basic_test() {
	clist list = clist_create();
	int x = 3;
	int y = 4;
	int z = 1;
	int w = 2;

	clist_add_back(list, &x);
	clist_add_back(list, &y);
	clist_add_back(list, &z);
	clist_add_back(list, &w);

	printf("the size of list is: %d\n", clist_size(list));

	while (clist_size(list) > 0) {
		int* obj = clist_remove_front(list);

		printf("removing %d\n", *obj);
	}

	printf("After removing list size : %d\n", clist_size(list));

	clist_destroy(list);
	return;
}

int int_compare(void* x, void* y) {
	int a = *((int*)x);
	int b = *((int*)y);

	if (a < b) return -1;
	else if (a > b) return 1;

	return 0;
}

void test_bubble_sort() {
	int x[50], i;
	clist pl = NULL;
	citer_dos(begin, NULL);
	citer_dos(end, NULL);
	citer_dos(itr, NULL);

	for (i = 0; i < 50; i ++) {
		x[i] = ((i*17351+73)%50+50)%50;
		printf("%d ", x[i]);
	}
	printf("\n\n");

	pl = clist_create(pl);

	for (i = 0; i < 50; i ++) {
		clist_add_back(pl, &x[i]);
	}

	clist_citer_begin(pl, begin);
	clist_citer_end(pl, end);
	citer_swap(begin, end);
	citer_for_each(begin, end, print);

	printf("\n\n");
	citer_to_next(begin);
	citer_to_prev(end);
	quick_sort(begin, end, int_compare);

	citer_for_each(begin, end, print);
	printf("\n\n");

	clist_destroy(pl);
}

// typedef void (*pf_param_void)(void* param);
// 
// void func2() {
// 	printf("func2\n");
// }
// 
// typedef void* type;
// 
// void function(type param) {
// 	printf("here we go: %d", *((int*)param));
// 	return func2();
// }
// 
// void test_grammar() {
// 	void* void_pointer;
// 	int integer = 5;
// 	int* int_pointer = &integer;
// 
// 	void_pointer = &integer;
// 
// 	void_pointer = int_pointer;
// 
// 	float floatnumber = 5.5;
// 	integer = floatnumber;
// 
// 	printf("%d\n", integer);
// 
// 	pf_param_void pf = function;
// 	pf(&integer);
// }

#include <cntr_abs.h>

void test_cntr_abs() {
	//printf("\n\ntest container begins\n\n");
	cntr c = clinear_as_array();
	citer_dos(begin, NULL);
	citer_dos(end, NULL);
	citer_dos(itr, NULL);
	int x[500], i;

	for (i = 0; i < 500; i ++) {
		x[i] = (12173*i+757)%147;
		//x[i] = 500 - i;
		printf("%d ", x[i]);
	}

	for (i = 0; i < 500; i ++) {
		clinear_add_front(c, &x[i]);
	}
	printf("container size is: %d\n", clinear_size(c));

	clinear_citer_begin(c, begin);
	clinear_citer_end(c, end);
	printf("\ninited : \n");
	citer_for_each(begin, end, print);
	printf("\n");

	citer_swap(begin, end);

	printf("\nafter swap: \n");
	citer_for_each(begin, end, print);
	printf("\n");

	printf("\nafter sort: \n");
	quick_sort(begin, end, int_compare);

	citer_for_each(begin, end, print);
	printf("\n");
	clinear_destroy(c);
}

int main() {
	basic_test();
	test_bubble_sort();
	//test_grammar();
	test_cntr_abs();

	return 0;
}

