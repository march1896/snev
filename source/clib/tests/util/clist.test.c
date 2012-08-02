#include <cntr_list.h>
#include <cntr_algorithm.h>
#include <cntr_iterator.h>

#include <stdio.h>

void basic_test() {
	clist list;
	int x = 3;
	int y = 4;
	int z = 1;
	int w = 2;

	clist_init(&list);

	clist_add_back(&list, &x);
	clist_add_back(&list, &y);
	clist_add_back(&list, &z);
	clist_add_back(&list, &w);

	printf("the size of list is: %d\n", list.size);

	while (list.size > 0) {
		int* obj = clist_remove_front(&list);

		printf("removing %d\n", *obj);
	}

	printf("After removing list size : %d\n", clist_size(&list));

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
	clist* pl;
	citer begin, end, itr;

	for (i = 0; i < 50; i ++) {
		x[i] = ((i*17351+73)%50+50)%50;
		printf("%d ", x[i]);
	}
	printf("\n\n");

	pl = (clist*)halloc(sizeof(clist));
	clist_init(pl);

	for (i = 0; i < 50; i ++) {
		clist_add_back(pl, &x[i]);
	}

	clist_citer_begin(pl, &itr);
	while (citer_valid(&itr)) {
		printf("%d ", *((int*)citer_get_ref(&itr)));

		citer_to_next(&itr);
	}
	printf("\n\n");

	clist_citer_begin(pl, &begin);
	clist_citer_end(pl, &end);
	citer_to_next(&begin);
	citer_to_prev(&end);
	quick_sort(&begin, &end, int_compare);

	clist_citer_begin(pl, &itr);
	while (citer_valid(&itr)) {
		printf("%d ", *((int*)citer_get_ref(&itr)));

		citer_to_next(&itr);
	}
	printf("\n\n");

	for_each(clist, pl, &itr) {
		printf("%d ", *((int*)citer_get_ref(&itr)));
	}
}

typedef void (*pf_param_void)(void* param);

void func2() {
	printf("func2\n");
}

typedef void* type;

void function(type param) {
	printf("here we go: %d", *((int*)param));
	return func2();
}

void test_grammar() {
	void* void_pointer;
	int integer = 5;
	int* int_pointer = &integer;

	void_pointer = &integer;

	void_pointer = int_pointer;

	float floatnumber = 5.5;
	integer = floatnumber;

	printf("%d\n", integer);

	pf_param_void pf = function;
	pf(&integer);
}

#include <cntr_abs.h>
void test_cntr_abs() {
	printf("\n\ntest container begins\n\n");
	container cntr = clinear_as_list();
	citer begin, end, itr;
	int x[50], i;

	for (i = 0; i < 50; i ++) {
		x[i] = (12173*i+757)%147;
		printf("%d ", x[i]);
	}

	printf("container size is: %d\n", clinear_size(cntr));

	for (i = 0; i < 50; i ++) {
		clinear_add_back(cntr, &x[i]);
	}

	for_each(clinear, cntr, &itr) {
		printf("%d ", *((int*)citer_get_ref(&itr)));
	}
	printf("\n");

	clinear_citer_begin(cntr, &begin);
	clinear_citer_end(cntr, &end);
	quick_sort(&begin, &end, int_compare);

	for_each(clinear, cntr, &itr) {
		printf("%d ", *((int*)citer_get_ref(&itr)));
	}
	printf("\n");
	clinear_deinit(cntr);
}

int main() {
	basic_test();
	test_bubble_sort();
	test_grammar();
	test_cntr_abs();

	return 0;
}
