#include "cntr2/istack.h"
#include "cntr2/iitr.h"

#include "istack.test.h"
#include "test_util.h"

static void stack_test_basic_itr_operation(iobject* stack) {
	istack_clear(stack);
	dbg_assert(istack_size(stack) == 0);

	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)2);
	istack_push(stack, (void*)(intptr_t)3);
	istack_push(stack, (void*)(intptr_t)4);

	/* now the stack contains { 1, 2, 3, 4 } */
	dbg_assert(istack_size(stack) == 4);
	{
		iterator itr = istack_itr_create(stack, itr_begin);
		const_iterator end = istack_itr_end(stack);
		intptr_t current = 1;

		/* traverse the stack */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)current);
			current ++;
		}

		/* test itr_assign */
		istack_itr_assign(stack, itr, itr_begin);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)1);
		istack_itr_assign(stack, itr, itr_end);
		itr_to_prev(itr);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)4);

		itr_destroy(itr);
	}

	istack_clear(stack);
	dbg_assert(istack_size(stack) == 0);

	/* test unique data */
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)1);

	/* now the stack contains eight 1s, { 1, 1, 1, 1, 1, 1, 1, 1 } */
	dbg_assert(istack_size(stack) == 8);
	{
		iterator itr = istack_itr_create(stack, itr_begin);
		const_iterator end = istack_itr_end(stack);

		/* traverse the stack */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)1);
		}

		itr_destroy(itr);
	}

	return;
}

static void stack_test_basic_operation(iobject* stack) {
	istack_clear(stack);
	dbg_assert(istack_size(stack) == 0);
	dbg_assert(istack_empty(stack));

	istack_push(stack, (void*)(intptr_t)1);
	istack_push(stack, (void*)(intptr_t)2);
	istack_push(stack, (void*)(intptr_t)3);
	istack_push(stack, (void*)(intptr_t)4);
	istack_push(stack, (void*)(intptr_t)5);
	istack_push(stack, (void*)(intptr_t)6);
	istack_push(stack, (void*)(intptr_t)7);
	istack_push(stack, (void*)(intptr_t)8);
	/* now the stack contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	/* test pop */
	dbg_assert(istack_size(stack) == 8);
	{
		/* now the stack contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t counter = 1;
		intptr_t element;

		dbg_assert(istack_top  (stack) == (void*)(intptr_t)8);

		for (counter = 8; counter >= 1; counter --) {
			element = (intptr_t)istack_pop(stack);
			dbg_assert(element == counter);
		}
	}

	/* test intersect push and pop */
	{
		intptr_t element = 0;

		dbg_assert(istack_empty(stack));
		istack_push(stack, (void*)(intptr_t)1);
		istack_push(stack, (void*)(intptr_t)2);
		/* push two element, now is { 1, 2 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 2);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)2);

		element = (intptr_t)istack_pop(stack);
		/* remove the top one, now is { 1 } */
		dbg_assert(element == 2);
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 1);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)1);

		istack_push(stack, (void*)(intptr_t)3);
		istack_push(stack, (void*)(intptr_t)4);
		/* push another 2 element, now is { 1, 3, 4 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 3);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)4);

		/* continous pop all of them */
		element = (intptr_t)istack_pop(stack);
		/* remove the top one, now is { 1, 3 } */
		dbg_assert(element == 4);
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 2);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)3);

		element = (intptr_t)istack_pop(stack);
		/* remove the top one, now is { 1 } */
		dbg_assert(element == 3);
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 1);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)1);

		element = (intptr_t)istack_pop(stack);
		/* remove the top one, now is {} */
		dbg_assert(element == 1);
		dbg_assert(istack_empty(stack) == true);
		dbg_assert(istack_size (stack) == 0);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)NULL);

		/* trying to pop empty stack */
		element = (intptr_t)istack_pop(stack);
		dbg_assert(element == (intptr_t)NULL);
		dbg_assert(istack_empty(stack) == true);
		dbg_assert(istack_size (stack) == 0);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)NULL);

		/* trying to push an element after pop empty stack */
		istack_push(stack, (void*)(intptr_t)1);
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 1);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)1);
	}

	/* test unique element */
	{
		intptr_t element = 0;
		istack_clear(stack);

		istack_push(stack, (void*)(intptr_t)1);
		istack_push(stack, (void*)(intptr_t)1);
		istack_push(stack, (void*)(intptr_t)1);
		/* now the stack is { 1, 1, 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 3);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)1);

		element = (intptr_t)istack_pop(stack);
		/* remove the top one, now is { 1, 1 } */
		dbg_assert(element == 1);
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 2);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)1);

		istack_push(stack, (void*)(intptr_t)1);
		/* now the stack is { 1, 1, 1 } */
		dbg_assert(istack_empty(stack) == false);
		dbg_assert(istack_size (stack) == 3);
		dbg_assert(istack_top  (stack) == (void*)(intptr_t)1);

		istack_clear(stack);
	}
}

void stack_test_basic(iobject* stack) {

	stack_test_basic_operation(stack);

	stack_test_basic_itr_operation(stack);
}

void stack_test_memory(iobject* stack) {
}

void stack_test_bench(iobject* stack) {
	/* should this exist, or it just compares with the raw stack_link to show the overhead of 'virtual' funtions */
}

