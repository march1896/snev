#include <cntr2/istack.h>
#include <cntr2/iitr.h>
#include <memheap/heap_global.h>

#include "istack.test.h"
#include "test_util.h"
#include "ele.data.h"

static void stack_test_basic_itr_operation(_object* stack, struct test_data_desc* td_desc) {
	address *test_data_addr   = td_desc->data_repo;
	unknown_traits *td_traits = td_desc->data_traits;
	istack_clear(stack);
	dbg_assert(istack_size(stack) == 0);

	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[2]);
	istack_push(stack, test_data_addr[3]);
	istack_push(stack, test_data_addr[4]);

	/* now the stack contains { 1, 2, 3, 4 } */
	dbg_assert(istack_size(stack) == 4);
	{
		iterator itr = istack_itr_create(stack, itr_begin);
		const_iterator end = istack_itr_end(stack);
		intptr_t current = 1;
		unknown* x = NULL;

		/* traverse the stack */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			x = (int*)itr_get_ref(itr);
			dbg_assert(td_traits->__equals(x, test_data_addr[current]));
			current ++;
		}

		/* test itr_assign */
		istack_itr_assign(stack, itr, itr_begin);
		x = (int*)itr_get_ref(itr);
		dbg_assert(td_traits->__equals(x, test_data_addr[1]));
		istack_itr_assign(stack, itr, itr_end);
		itr_to_prev(itr);
		x = (int*)itr_get_ref(itr);
		dbg_assert(td_traits->__equals(x, test_data_addr[4]));

		itr_destroy(itr);
	}

	istack_clear(stack);
	dbg_assert(istack_size(stack) == 0);

	/* test unique data */

	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[1]);

	/* now the stack contains eight 1s, { 1, 1, 1, 1, 1, 1, 1, 1 } */
	dbg_assert(istack_size(stack) == 8);
	{
		iterator itr = istack_itr_create(stack, itr_begin);
		const_iterator end = istack_itr_end(stack);

		/* traverse the stack */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			unknown* x = (int*)itr_get_ref(itr);
			dbg_assert(td_traits->__equals(x, test_data_addr[1]));
		}

		itr_destroy(itr);
	}

	return;
}

static void stack_test_basic_operation(_object* stack, struct test_data_desc* td_desc) {
	address *test_data_addr   = td_desc->data_repo;
	unknown_traits *td_traits = td_desc->data_traits;
	istack_clear(stack);
	dbg_assert(istack_size(stack) == 0);
	dbg_assert(istack_empty(stack));


	istack_push(stack, test_data_addr[1]);
	istack_push(stack, test_data_addr[2]);
	istack_push(stack, test_data_addr[3]);
	istack_push(stack, test_data_addr[4]);
	istack_push(stack, test_data_addr[5]);
	istack_push(stack, test_data_addr[6]);
	istack_push(stack, test_data_addr[7]);
	istack_push(stack, test_data_addr[8]);
	/* now the stack contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	/* test pop */
	dbg_assert(istack_size(stack) == 8);
	{
		/* now the stack contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t counter = 1;

		dbg_assert(td_traits->__equals(istack_top (stack), test_data_addr[8]));

		for (counter = 8; counter >= 1; counter --) {
			istack_pop(stack);
		}
	}

	/* test intersect push and pop */
	{
		dbg_assert(istack_empty(stack));

		istack_push(stack, test_data_addr[1]);
		istack_push(stack, test_data_addr[2]);
		/* push two element, now is { 1, 2 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 2);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[2]));

		istack_pop(stack);
		/* remove the top one, now is { 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 1);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[1]));


		istack_push(stack, test_data_addr[3]);
		istack_push(stack, test_data_addr[4]);
		/* push another 2 element, now is { 1, 3, 4 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 3);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[4]));

		/* continuous pop all of them */
		istack_pop(stack);
		/* remove the top one, now is { 1, 3 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 2);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[3]));

		istack_pop(stack);
		/* remove the top one, now is { 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 1);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[1]));

		istack_pop(stack);
		/* remove the top one, now is {} */
		dbg_assert(istack_empty(stack) == true);
		dbg_assert(istack_size (stack) == 0);
		dbg_assert(istack_top  (stack) == NULL);

		/* trying to pop empty stack */
		istack_pop(stack);
		dbg_assert(istack_empty(stack) == true);
		dbg_assert(istack_size (stack) == 0);
		dbg_assert(istack_top  (stack) == NULL);

		/* trying to push an element after pop empty stack */

		istack_push(stack, test_data_addr[1]);
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 1);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[1]));
	}

	/* test unique element */
	{
		istack_clear(stack);


		istack_push(stack, test_data_addr[1]);
		istack_push(stack, test_data_addr[1]);
		istack_push(stack, test_data_addr[1]);
		/* now the stack is { 1, 1, 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 3);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[1]));

		istack_pop(stack);
		/* remove the top one, now is { 1, 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 2);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[1]));

		istack_push(stack, test_data_addr[1]);
		/* now the stack is { 1, 1, 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 3);
		dbg_assert(td_traits->__equals(istack_top(stack), test_data_addr[1]));

		istack_clear(stack);
	}
}

void stack_test_basic(_object* a, struct test_data_desc* td_desc) {
	stack_test_basic_operation(a, td_desc);
	stack_test_basic_itr_operation(a, td_desc);
}

void stack_test_memory(istack stack) {
}

void stack_test_bench(istack stack) {
	/* should this exist, or it just compares with the raw stack_link to show the overhead of 'virtual' funtions */
}

