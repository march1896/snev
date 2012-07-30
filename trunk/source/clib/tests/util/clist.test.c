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
	bubble_sort(&begin, &end, int_compare);

	clist_citer_begin(pl, &itr);
	while (citer_valid(&itr)) {
		printf("%d ", *((int*)citer_get_ref(&itr)));

		citer_to_next(&itr);
	}
	printf("\n\n");
// 	for_each(clist, *pl, itr) {
// 		printf("%d ", *((int*)citer_get_ref(&itr)));
// 	}
}

int main() {
	basic_test();
	test_bubble_sort();

	return 0;
}
