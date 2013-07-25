#include <test_util.h>
#include <stdio.h>
#include <string.h>

#include <ilist.h>
#include <iqueue.h>
#include <istack.h>
#include <iitr.h>
#include <ialgo.h>
#include <ifactory.h>

static void __ref_print(void* __ref) {
	printf("%d ", (int)__ref);
}
static void __odlist_print(struct base_interface* ic) {
	struct base_interface* iq = as_queue(ic);
	iterator begin = iqueue_itr_begin(iq);
	iterator end   = iqueue_itr_end(iq);

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

	ilist_destroy(list);
}


static void __queue_correct() {
	struct base_interface* iq = as_queue(create_dblinked_list());

	int a[10], i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}

	__odlist_print(iq);
	iqueue_push(iq, (void*)a[0]); /* { 0 } */
	__odlist_print(iq);
	iqueue_push(iq, (void*)a[1]); /* { 0, 1 } */
	__odlist_print(iq);
	iqueue_push(iq, (void*)a[2]); /* { 0, 1, 2 } */
	__odlist_print(iq);
	iqueue_push(iq, (void*)a[3]); /* { 0, 1, 2, 3 } */
	__odlist_print(iq);
	iqueue_push(iq, (void*)a[4]); /* { 0, 1, 2, 3, 4 } */
	__odlist_print(iq);
	iqueue_push(iq, (void*)a[5]); /* { 0, 1, 2, 3, 4, 5} */
	__odlist_print(iq);

	dbg_assert(iqueue_size(iq) == 6);

	__ref = (int)iqueue_pop(iq); /* { 1, 2, 3, 4, 5} */
	__odlist_print(iq);
	dbg_assert(__ref == 0);
	__ref = (int)iqueue_pop(iq); /* { 2, 3, 4, 5 } */
	__odlist_print(iq);
	dbg_assert(__ref == 1);
	__ref = (int)iqueue_pop(iq); /* { 3, 4, 5 } */
	__odlist_print(iq);
	dbg_assert(__ref == 2);
	__ref = (int)iqueue_pop(iq); /* { 4, 5 } */
	__odlist_print(iq);
	dbg_assert(__ref == 3);
	__ref = (int)iqueue_pop(iq); /* { 5 } */
	__odlist_print(iq);
	dbg_assert(__ref == 4);

	dbg_assert(iqueue_size(iq) == 1);

	iqueue_destroy(iq);
}

static void __stack_correct() {
	struct base_interface* iq = as_stack(create_dblinked_list());

	int a[10], i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}
	__odlist_print(iq);
	istack_push(iq, (void*)a[0]); /* { 0 } */
	__odlist_print(iq);
	istack_push(iq, (void*)a[1]); /* { 0, 1 } */
	__odlist_print(iq);
	istack_push(iq, (void*)a[2]); /* { 0, 1, 2 } */
	__odlist_print(iq);
	istack_push(iq, (void*)a[3]); /* { 0, 1, 2, 3 } */
	__odlist_print(iq);
	istack_push(iq, (void*)a[4]); /* { 0, 1, 2, 3, 4 } */
	__odlist_print(iq);
	istack_push(iq, (void*)a[5]); /* { 0, 1, 2, 3, 4, 5} */
	__odlist_print(iq);

	dbg_assert(istack_size(iq) == 6);

	__ref = (int)istack_pop(iq); /* { 0, 1, 2, 3, 4} */
	__odlist_print(iq);
	dbg_assert(__ref == 5);
	__ref = (int)istack_pop(iq); /* { 0, 1, 2, 3 } */
	__odlist_print(iq);
	dbg_assert(__ref == 4);
	__ref = (int)istack_pop(iq); /* { 0, 1, 2 } */
	__odlist_print(iq);
	dbg_assert(__ref == 3);
	__ref = (int)istack_pop(iq); /* { 0, 1 } */
	__odlist_print(iq);
	dbg_assert(__ref == 2);
	__ref = (int)istack_pop(iq); /* { 0 } */
	__odlist_print(iq);
	dbg_assert(__ref == 1);

	dbg_assert(istack_size(iq) == 1);

	istack_destroy(iq);
}

static void __correctness_test() {
	printf("add find remove correctness test start\n");
	__list_correct();
	__queue_correct();
	__stack_correct();
	printf("add find remove correctness test end\n");
}

static void __performance_test() {
	printf("add find remove performance test start\n");
	printf("add find remove performance test end\n");
}

void cntr2_odlist_test() {
	do_test("cntr cntr_base correctness", __correctness_test);

	do_test("cntr cntr_base performance", __performance_test);
}
