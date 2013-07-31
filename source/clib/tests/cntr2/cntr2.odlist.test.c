#include <stdio.h>
#include <string.h>

#include <ilist.h>
#include <iqueue.h>
#include <istack.h>
#include <iset.h>
#include <iitr.h>
#include <ialgo.h>
#include <ifactory.h>

static void __ref_print(void* __ref) {
	printf("%d ", (int)__ref);
}
static void __odlist_print(struct base_interface* ic) {
	ilist list = as_list(ic);
	iterator begin = ilist_itr_begin(list);
	iterator end   = ilist_itr_end(list);

	while (!itr_equals(begin, end)) {
		__ref_print(itr_get_ref(begin));
		itr_to_next(begin);
	}
	foreach(begin, end, __ref_print);

	printf("\n");
	itr_destroy(begin);
	itr_destroy(end);
}

static void __list_correct() {
	ilist list = as_list(create_dblinked_list());

	int a[10], i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}

	__odlist_print(list);               /* { } */
	ilist_add_back(list, (void*)a[0]);  /* { 0 } */
	__odlist_print(list);
	ilist_add_front(list, (void*)a[1]); /* { 1, 0 } */
	__odlist_print(list);
	ilist_add_back(list, (void*)a[2]);  /* { 1, 0, 2 } */
	__odlist_print(list);
	ilist_add_front(list, (void*)a[3]); /* { 3, 1, 0, 2 } */
	__odlist_print(list);
	ilist_add_back(list, (void*)a[4]);  /* { 3, 1, 0, 2, 4 } */
	__odlist_print(list);
	ilist_add_front(list, (void*)a[5]); /* { 5, 3, 1, 0, 2, 4 } */
	__odlist_print(list);

	for (i = 0; i <= 5; i ++) {
		iterator itr = ilist_itr_find(list, (void*)a[i]);
		iterator end = ilist_itr_end(list);

		dbg_assert(!itr_equals(itr, end));
		dbg_assert(itr_get_ref(itr) == (void*)a[i]);

		itr_destroy(itr);
		itr_destroy(end);
	}
	for (i = 6; i < 10; i ++) {
		iterator itr = ilist_itr_find(list, (void*)a[i]);
		iterator end = ilist_itr_end(list);

		dbg_assert(itr_equals(itr, end));

		itr_destroy(itr);
		itr_destroy(end);
	}
	
	dbg_assert(ilist_size(list) == 6);
// 
//  	__ref = (int)ilist_remove_back(list); /* { 3, 1, 0, 2, 4 } */
//  	dbg_assert(__ref == 4);
//  	__ref = (int)ilist_remove_back(list); /* { 1, 0, 2, 4 } */
//  	dbg_assert(__ref == 2);

	__odlist_print(list);

	ilist_destroy(list);
}


static void __queue_correct() {
	iqueue queue = as_queue(create_dblinked_list());

	int a[10], i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}

	__odlist_print(queue);
	iqueue_push(queue, (void*)a[0]); /* { 0 } */
	__odlist_print(queue);
 	iqueue_push(queue, (void*)a[1]); /* { 0, 1 } */
 	__odlist_print(queue);
	iqueue_push(queue, (void*)a[2]); /* { 0, 1, 2 } */
	__odlist_print(queue);
	iqueue_push(queue, (void*)a[3]); /* { 0, 1, 2, 3 } */
	__odlist_print(queue);
	iqueue_push(queue, (void*)a[4]); /* { 0, 1, 2, 3, 4 } */
	__odlist_print(queue);
	iqueue_push(queue, (void*)a[5]); /* { 0, 1, 2, 3, 4, 5} */
	__odlist_print(queue);

	dbg_assert(iqueue_size(queue) == 6);

	__ref = (int)iqueue_pop(queue); /* { 1, 2, 3, 4, 5} */
	//__odlist_print(queue);
	dbg_assert(__ref == 0);
	__ref = (int)iqueue_pop(queue); /* { 2, 3, 4, 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 1);
	__ref = (int)iqueue_pop(queue); /* { 3, 4, 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 2);
	__ref = (int)iqueue_pop(queue); /* { 4, 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 3);
	__ref = (int)iqueue_pop(queue); /* { 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 4);

	dbg_assert(iqueue_size(queue) == 1);

	iqueue_destroy(queue);
}

static void __stack_correct() {
	istack stack = as_stack(create_dblinked_list());

	int a[10], i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}
	__odlist_print(stack);
	istack_push(stack, (void*)a[0]); /* { 0 } */
	__odlist_print(stack);
	istack_push(stack, (void*)a[1]); /* { 0, 1 } */
	__odlist_print(stack);
	istack_push(stack, (void*)a[2]); /* { 0, 1, 2 } */
	__odlist_print(stack);
	istack_push(stack, (void*)a[3]); /* { 0, 1, 2, 3 } */
	__odlist_print(stack);
	istack_push(stack, (void*)a[4]); /* { 0, 1, 2, 3, 4 } */
	__odlist_print(stack);
	istack_push(stack, (void*)a[5]); /* { 0, 1, 2, 3, 4, 5} */
	__odlist_print(stack);

	dbg_assert(istack_size(stack) == 6);

	__ref = (int)istack_pop(stack); /* { 0, 1, 2, 3, 4} */
	__odlist_print(stack);
	dbg_assert(__ref == 5);
	__ref = (int)istack_pop(stack); /* { 0, 1, 2, 3 } */
	__odlist_print(stack);
	dbg_assert(__ref == 4);
	__ref = (int)istack_pop(stack); /* { 0, 1, 2 } */
	__odlist_print(stack);
	dbg_assert(__ref == 3);
	__ref = (int)istack_pop(stack); /* { 0, 1 } */
	__odlist_print(stack);
	dbg_assert(__ref == 2);
	__ref = (int)istack_pop(stack); /* { 0 } */
	__odlist_print(stack);
	dbg_assert(__ref == 1);

	dbg_assert(istack_size(stack) == 1);

	istack_destroy(stack);
}

static void __set_correct() {
	iset set = as_set(create_dblinked_list());

	int a[10], i;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}

	for (i = 0; i < 10; i ++) {
		if (i % 2 == 0) {
			iset_insert(set, (void*)a[i]);
		}
	}

	dbg_assert(iset_size(set) == 5);

	for (i = 0; i < 10; i ++) {
		iterator itr = iset_find(set, (void*)a[i]);
		iterator end = iset_itr_end(set);

		if (i % 2 == 0) {
			dbg_assert(!itr_equals(itr, end));
			dbg_assert(itr_get_ref(itr) == (void*)a[i]);
		}
		else {
			dbg_assert(itr_equals(itr, end));
		}

		itr_destroy(itr);
		itr_destroy(end);
	}

	iset_destroy(set);
}

static void __correctness_test() {
	printf("add find remove correctness test start\n");
	__list_correct();
	__queue_correct();
	__stack_correct();
	__set_correct();
	printf("add find remove correctness test end\n");
}

static void __performance_test() {
	printf("add find remove performance test start\n");
	printf("add find remove performance test end\n");
}

#define temp_do_test(test_name, test_function) \
	printf("%s test start\n", test_name);\
	test_function(); \
	printf("%s test end\n\n", test_name);

void cntr2_odlist_test() {
	temp_do_test("cntr cntr_base correctness", __correctness_test);

	temp_do_test("cntr cntr_base performance", __performance_test);
}
