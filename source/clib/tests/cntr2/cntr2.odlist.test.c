#include <test_util.h>
#include <stdio.h>
#include <string.h>

#include <iqueue.h>
#include <istack.h>
#include <iitr.h>
#include <ifactory.h>

static void __odlist_print(struct base_interface* ic) {
	/* TODO: use the algorithm for_each method */
	struct base_interface* iq = as_queue(ic);

	iobject* begin = as_bitr(iqueue_itr_begin(iq));
	iobject* end   = as_bitr(iqueue_itr_end(iq));

	while (!ibitr_equals(begin, end)) {
		printf("%d ", (int)ibitr_get_ref(begin));
		ibitr_to_next(begin);
	}

	printf("%d ", (int)ibitr_get_ref(begin));
	printf("\n");

	ibitr_destroy(begin);
	ibitr_destroy(end);
}

static void __queue_correct() {
	struct base_interface* iq = as_queue(create_dblinked_list());

	int a[10], i, ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}

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

	ref = (int)iqueue_pop(iq); /* { 1, 2, 3, 4, 5} */
	__odlist_print(iq);
	dbg_assert(ref == 0);
	ref = (int)iqueue_pop(iq); /* { 2, 3, 4, 5 } */
	__odlist_print(iq);
	dbg_assert(ref == 1);
	ref = (int)iqueue_pop(iq); /* { 3, 4, 5 } */
	__odlist_print(iq);
	dbg_assert(ref == 2);
	ref = (int)iqueue_pop(iq); /* { 4, 5 } */
	__odlist_print(iq);
	dbg_assert(ref == 3);
	ref = (int)iqueue_pop(iq); /* { 5 } */
	__odlist_print(iq);
	dbg_assert(ref == 4);

	dbg_assert(iqueue_size(iq) == 1);

	iqueue_destroy(iq);
}

static void __stack_correct() {
	struct base_interface* iq = as_stack(create_dblinked_list());

	int a[10], i, ref;
	for (i = 0; i < 10; i ++) {
		a[i] = i;
	}

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

	ref = (int)istack_pop(iq); /* { 0, 1, 2, 3, 4} */
	__odlist_print(iq);
	dbg_assert(ref == 5);
	ref = (int)istack_pop(iq); /* { 0, 1, 2, 3 } */
	__odlist_print(iq);
	dbg_assert(ref == 4);
	ref = (int)istack_pop(iq); /* { 0, 1, 2 } */
	__odlist_print(iq);
	dbg_assert(ref == 3);
	ref = (int)istack_pop(iq); /* { 0, 1 } */
	__odlist_print(iq);
	dbg_assert(ref == 2);
	ref = (int)istack_pop(iq); /* { 0 } */
	__odlist_print(iq);
	dbg_assert(ref == 1);

	dbg_assert(istack_size(iq) == 1);

	istack_destroy(iq);
}

static void __correctness_test() {
	printf("add find remove correctness test start\n");
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
