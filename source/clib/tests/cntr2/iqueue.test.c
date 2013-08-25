#include "cntr2/iqueue.h"
#include "cntr2/iitr.h"

#include "iqueue.test.h"
#include "test_util.h"

static void queue_test_basic_itr_operation(iobject* queue) {
	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);

	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)2);
	iqueue_push(queue, (void*)(intptr_t)3);
	iqueue_push(queue, (void*)(intptr_t)4);

	/* now the queue contains { 1, 2, 3, 4 } */
	dbg_assert(iqueue_size(queue) == 4);
	{
		iterator itr = iqueue_itr_create(queue, itr_begin);
		const_iterator end = iqueue_itr_end(queue);
		intptr_t current = 1;

		/* traverse the queue */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)current);
			current ++;
		}

		/* test itr_assign */
		iqueue_itr_assign(queue, itr, itr_begin);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)1);
		iqueue_itr_assign(queue, itr, itr_end);
		itr_to_prev(itr);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)4);

		itr_destroy(itr);
	}

	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);

	/* test unique data */
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)1);

	/* now the queue contains eight 1s, { 1, 1, 1, 1, 1, 1, 1, 1 } */
	dbg_assert(iqueue_size(queue) == 8);
	{
		iterator itr = iqueue_itr_create(queue, itr_begin);
		const_iterator end = iqueue_itr_end(queue);

		/* traverse the queue */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)1);
		}

		itr_destroy(itr);
	}

	return;
}

static void queue_test_basic_operation(iobject* queue) {
	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);
	dbg_assert(iqueue_empty(queue));

	iqueue_push(queue, (void*)(intptr_t)1);
	iqueue_push(queue, (void*)(intptr_t)2);
	iqueue_push(queue, (void*)(intptr_t)3);
	iqueue_push(queue, (void*)(intptr_t)4);
	iqueue_push(queue, (void*)(intptr_t)5);
	iqueue_push(queue, (void*)(intptr_t)6);
	iqueue_push(queue, (void*)(intptr_t)7);
	iqueue_push(queue, (void*)(intptr_t)8);
	/* now the queue contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	/* test pop */
	dbg_assert(iqueue_size(queue) == 8);
	{
		/* now the queue contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t counter = 1;
		intptr_t element;

		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)1);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)8);

		for (counter = 1; counter <= 8; counter ++) {
			element = (intptr_t)iqueue_pop(queue);
			dbg_assert(element == counter);
		}
	}

	/* test intersect push and pop */
	{
		intptr_t element = 0;

		dbg_assert(iqueue_empty(queue));
		iqueue_push(queue, (void*)(intptr_t)1);
		iqueue_push(queue, (void*)(intptr_t)2);
		/* push two element, now is { 1, 2 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 2);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)1);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)2);

		element = (intptr_t)iqueue_pop(queue);
		/* remove the front one, now is { 2 } */
		dbg_assert(element == 1);
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 1);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)2);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)2);

		iqueue_push(queue, (void*)(intptr_t)3);
		iqueue_push(queue, (void*)(intptr_t)4);
		/* push another 2 element, now is { 2, 3, 4 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 3);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)2);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)4);

		/* continous pop all of them */
		element = (intptr_t)iqueue_pop(queue);
		/* remove the front one, now is { 3, 4 } */
		dbg_assert(element == 2);
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 2);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)3);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)4);

		element = (intptr_t)iqueue_pop(queue);
		/* remove the front one, now is { 4 } */
		dbg_assert(element == 3);
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 1);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)4);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)4);

		element = (intptr_t)iqueue_pop(queue);
		/* remove the front one, now is {} */
		dbg_assert(element == 4);
		dbg_assert(iqueue_empty(queue) == true);
		dbg_assert(iqueue_size (queue) == 0);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)NULL);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)NULL);

		/* trying to pop empty queue */
		element = (intptr_t)iqueue_pop(queue);
		dbg_assert(element == (intptr_t)NULL);
		dbg_assert(iqueue_empty(queue) == true);
		dbg_assert(iqueue_size (queue) == 0);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)NULL);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)NULL);

		/* trying to push an element after pop empty queue */
		iqueue_push(queue, (void*)(intptr_t)1);
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 1);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)1);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)1);
	}

	/* test unique element */
	{
		intptr_t element = 0;
		iqueue_clear(queue);

		iqueue_push(queue, (void*)(intptr_t)1);
		iqueue_push(queue, (void*)(intptr_t)1);
		iqueue_push(queue, (void*)(intptr_t)1);
		/* now the queue is { 1, 1, 1 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 3);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)1);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)1);

		element = (intptr_t)iqueue_pop(queue);
		/* remove the front one, now is { 1, 1 } */
		dbg_assert(element == 1);
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 2);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)1);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)1);

		iqueue_push(queue, (void*)(intptr_t)1);
		/* now the queue is { 1, 1, 1 } */
		dbg_assert(iqueue_empty(queue) == false);
		dbg_assert(iqueue_size (queue) == 3);
		dbg_assert(iqueue_front(queue) == (void*)(intptr_t)1);
		dbg_assert(iqueue_back (queue) == (void*)(intptr_t)1);

		iqueue_clear(queue);
	}
}

void queue_test_basic(iobject* queue) {

	queue_test_basic_operation(queue);

	queue_test_basic_itr_operation(queue);
}

void queue_test_memory(iobject* queue) {
}

void queue_test_bench(iobject* queue) {
	/* should this exist, or it just compares with the raw queue_link to show the overhead of 'virtual' funtions */
}

