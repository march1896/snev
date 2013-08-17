#include <stdio.h>
#include <string.h>

#include <ilist.h>
#include <iqueue.h>
#include <istack.h>
#include <iset.h>
#include <iitr.h>
#include <ialgo.h>
#include <ifactory.h>

#include <test_util.h>

#include <stdint.h>

static void __ref_print(void* __ref) {
	log_printf("%d ", (int)(intptr_t)__ref);
}
static void __odlist_print(struct base_interface* ic) {
	ilist list = as_list(ic);
	iterator begin = ilist_itr_begin(list);
	iterator end   = ilist_itr_end(list);
	log_printtab();

	while (!itr_equals(begin, end)) {
		__ref_print(itr_get_ref(begin));
		itr_to_next(begin);
	}
	foreach(begin, end, __ref_print);

	log_printf("\n");
	itr_destroy(begin);
	itr_destroy(end);
}

static void __list_correct() {
	ilist list = as_list(create_dblinked_list());

	void* a[10];
    int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}

	__odlist_print(list);               /* { } */
	ilist_add_back(list, a[0]);  /* { 0 } */
	__odlist_print(list);
	ilist_add_front(list, a[1]); /* { 1, 0 } */
	__odlist_print(list);
	ilist_add_back(list, a[2]);  /* { 1, 0, 2 } */
	__odlist_print(list);
	ilist_add_front(list, a[3]); /* { 3, 1, 0, 2 } */
	__odlist_print(list);
	ilist_add_back(list, a[4]);  /* { 3, 1, 0, 2, 4 } */
	__odlist_print(list);
	ilist_add_front(list, a[5]); /* { 5, 3, 1, 0, 2, 4 } */
	__odlist_print(list);

	for (i = 0; i <= 5; i ++) {
		iterator itr = ilist_itr_find(list, a[i]);
		iterator end = ilist_itr_end(list);

		dbg_assert(!itr_equals(itr, end));
		dbg_assert(itr_get_ref(itr) == a[i]);

		itr_destroy(itr);
		itr_destroy(end);
	}
	for (i = 6; i < 10; i ++) {
		iterator itr = ilist_itr_find(list, a[i]);
		iterator end = ilist_itr_end(list);

		dbg_assert(itr_equals(itr, end));

		itr_destroy(itr);
		itr_destroy(end);
	}
	
	dbg_assert(ilist_size(list) == 6);

 	__ref = (int)(intptr_t)ilist_remove_back(list); /* { 3, 1, 0, 2, 4 } */
 	dbg_assert(__ref == 4);
 	__ref = (int)(intptr_t)ilist_remove_back(list); /* { 1, 0, 2, 4 } */
 	dbg_assert(__ref == 2);

	__odlist_print(list);

	ilist_destroy(list);
}


static void __queue_correct() {
	iqueue queue = as_queue(create_dblinked_list());

	void* a[10];
	int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}

	__odlist_print(queue);
	iqueue_push(queue, a[0]); /* { 0 } */
	__odlist_print(queue);
 	iqueue_push(queue, a[1]); /* { 0, 1 } */
 	__odlist_print(queue);
	iqueue_push(queue, a[2]); /* { 0, 1, 2 } */
	__odlist_print(queue);
	iqueue_push(queue, a[3]); /* { 0, 1, 2, 3 } */
	__odlist_print(queue);
	iqueue_push(queue, a[4]); /* { 0, 1, 2, 3, 4 } */
	__odlist_print(queue);
	iqueue_push(queue, a[5]); /* { 0, 1, 2, 3, 4, 5} */
	__odlist_print(queue);

	dbg_assert(iqueue_size(queue) == 6);

	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 1, 2, 3, 4, 5} */
	//__odlist_print(queue);
	dbg_assert(__ref == 0);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 2, 3, 4, 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 1);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 3, 4, 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 2);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 4, 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 3);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 5 } */
	//__odlist_print(queue);
	dbg_assert(__ref == 4);

	dbg_assert(iqueue_size(queue) == 1);

	iqueue_destroy(queue);
}

static void __stack_correct() {
	istack stack = as_stack(create_dblinked_list());

    void* a[10];
	int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}
	__odlist_print(stack);
	istack_push(stack, a[0]); /* { 0 } */
	__odlist_print(stack);
	istack_push(stack, a[1]); /* { 0, 1 } */
	__odlist_print(stack);
	istack_push(stack, a[2]); /* { 0, 1, 2 } */
	__odlist_print(stack);
	istack_push(stack, a[3]); /* { 0, 1, 2, 3 } */
	__odlist_print(stack);
	istack_push(stack, a[4]); /* { 0, 1, 2, 3, 4 } */
	__odlist_print(stack);
	istack_push(stack, a[5]); /* { 0, 1, 2, 3, 4, 5} */
	__odlist_print(stack);

	dbg_assert(istack_size(stack) == 6);

	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1, 2, 3, 4} */
	__odlist_print(stack);
	dbg_assert(__ref == 5);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1, 2, 3 } */
	__odlist_print(stack);
	dbg_assert(__ref == 4);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1, 2 } */
	__odlist_print(stack);
	dbg_assert(__ref == 3);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1 } */
	__odlist_print(stack);
	dbg_assert(__ref == 2);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0 } */
	__odlist_print(stack);
	dbg_assert(__ref == 1);

	dbg_assert(istack_size(stack) == 1);

	istack_destroy(stack);
}

static void __set_correct() {
	iset set = as_set(create_dblinked_list());

    void* a[10];
	int i;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}

	for (i = 0; i < 10; i ++) {
		if (i % 2 == 0) {
			iset_insert(set, a[i]);
		}
	}

	dbg_assert(iset_size(set) == 5);

	for (i = 0; i < 10; i ++) {
		iterator itr = iset_find(set, a[i]);
		iterator end = iset_itr_end(set);

		if (i % 2 == 0) {
			dbg_assert(!itr_equals(itr, end));
			dbg_assert(itr_get_ref(itr) == a[i]);
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
	__list_correct();
	__queue_correct();
	__stack_correct();
	__set_correct();
}

static void __performance_test() {
	log_printline("add find remove performance test start");
	log_printline("add find remove performance test end");
}

void cntr2_odlist_test() {
	test_run_single("cntr cntr_base correctness", __correctness_test);

	test_run_single("cntr cntr_base performance", __performance_test);
}
