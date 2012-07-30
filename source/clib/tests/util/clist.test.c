#include <cntr_list.h>

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

	return;
}

int main() {
	basic_test();

	return 0;
}
