#include <cntr2/iqueue.h>
#include <cntr2/iitr.h>
#include <memheap/heap_global.h>

#include "iqueue.test.h"
#include "test_util.h"
#include "ele.data.h"

static void queue_test_basic_itr_operation(_object* queue, struct test_data_desc* td_desc) {
	address *test_data_addr   = td_desc->data_repo;
	unknown_traits *td_traits = td_desc->data_traits;
	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);


	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[2]);
	iqueue_push(queue, test_data_addr[3]);
	iqueue_push(queue, test_data_addr[4]);

	/* now the queue contains { 1, 2, 3, 4 } */
	dbg_assert(iqueue_size(queue) == 4);
	{
		iterator itr = iqueue_itr_create(queue, itr_begin);
		const_iterator end = iqueue_itr_end(queue);
		intptr_t current = 1;
		const unknown* x = NULL;

		/* traverse the queue */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			x = itr_get_ref(itr);
			dbg_assert(td_traits->__equals(x, test_data_addr[current]));
			current ++;
		}

		/* test itr_assign */
		iqueue_itr_assign(queue, itr, itr_begin);
		x = (int*)itr_get_ref(itr);
		dbg_assert(td_traits->__equals(x, test_data_addr[1]));
		iqueue_itr_assign(queue, itr, itr_end);
		itr_to_prev(itr);
		x = (int*)itr_get_ref(itr);
		dbg_assert(td_traits->__equals(x, test_data_addr[4]));

		itr_destroy(itr);
	}

	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);

	/* test unique data */

	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[1]);

	/* now the queue contains eight 1s, { 1, 1, 1, 1, 1, 1, 1, 1 } */
	dbg_assert(iqueue_size(queue) == 8);
	{
		iterator itr = iqueue_itr_create(queue, itr_begin);
		const_iterator end = iqueue_itr_end(queue);

		/* traverse the queue */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			unknown* x = (int*)itr_get_ref(itr);
			dbg_assert(td_traits->__equals(x, test_data_addr[1]));
		}

		itr_destroy(itr);
	}

	return;
}

static void queue_test_basic_operation(_object* queue, struct test_data_desc* td_desc) {
	address *test_data_addr   = td_desc->data_repo;
	unknown_traits *td_traits = td_desc->data_traits;
	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);
	dbg_assert(iqueue_empty(queue));


	iqueue_push(queue, test_data_addr[1]);
	iqueue_push(queue, test_data_addr[2]);
	iqueue_push(queue, test_data_addr[3]);
	iqueue_push(queue, test_data_addr[4]);
	iqueue_push(queue, test_data_addr[5]);
	iqueue_push(queue, test_data_addr[6]);
	iqueue_push(queue, test_data_addr[7]);
	iqueue_push(queue, test_data_addr[8]);
	/* now the queue contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	/* test pop */
	dbg_assert(iqueue_size(queue) == 8);
	{
		/* now the queue contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t counter = 1;

		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[8]));

		for (counter = 1; counter <= 8; counter ++) {
			iqueue_pop(queue);
		}
	}

	/* test intersect push and pop */
	{
		dbg_assert(iqueue_empty(queue));

		iqueue_push(queue, test_data_addr[1]);
		iqueue_push(queue, test_data_addr[2]);
		/* push two element, now is { 1, 2 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 2);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[2]));

		iqueue_pop(queue);
		/* remove the front one, now is { 2 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 1);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[2]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[2]));


		iqueue_push(queue, test_data_addr[3]);
		iqueue_push(queue, test_data_addr[4]);
		/* push another 2 element, now is { 2, 3, 4 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 3);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[2]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[4]));

		/* continuous pop all of them */
		iqueue_pop(queue);
		/* remove the front one, now is { 3, 4 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 2);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[3]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[4]));

		iqueue_pop(queue);
		/* remove the front one, now is { 4 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 1);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[4]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[4]));

		iqueue_pop(queue);
		/* remove the front one, now is {} */
		dbg_assert(iqueue_empty(queue) == true);
		dbg_assert(iqueue_size (queue) == 0);
		dbg_assert(iqueue_front(queue) == NULL);
		dbg_assert(iqueue_back (queue) == NULL);

		/* trying to pop empty queue */
		iqueue_pop(queue);
		dbg_assert(iqueue_empty(queue) == true);
		dbg_assert(iqueue_size (queue) == 0);
		dbg_assert(iqueue_front(queue) == NULL);
		dbg_assert(iqueue_back (queue) == NULL);

		/* trying to push an element after pop empty queue */
		iqueue_push(queue, test_data_addr[1]);
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 1);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[1]));
	}

	/* test unique element */
	{
		iqueue_clear(queue);

		iqueue_push(queue, test_data_addr[1]);
		iqueue_push(queue, test_data_addr[1]);
		iqueue_push(queue, test_data_addr[1]);
		/* now the queue is { 1, 1, 1 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 3);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[1]));

		iqueue_pop(queue);
		/* remove the front one, now is { 1, 1 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 2);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[1]));

		iqueue_push(queue, test_data_addr[1]);
		/* now the queue is { 1, 1, 1 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 3);
		dbg_assert(td_traits->__equals(iqueue_front(queue), test_data_addr[1]));
		dbg_assert(td_traits->__equals(iqueue_back (queue), test_data_addr[1]));

		iqueue_clear(queue);
	}
}

void queue_test_basic(_object* a, struct test_data_desc* td_desc) {
	queue_test_basic_operation(a, td_desc);
	queue_test_basic_itr_operation(a, td_desc);
}

void queue_test_memory(iqueue queue) {
}

void queue_test_bench(iqueue queue) {
	/* should this exist, or it just compares with the raw queue_link to show the overhead of 'virtual' funtions */
}

