#include "cntr2/ilist.h"
#include "cntr2/iitr.h"

#include "ilist.test.h"
#include "test_util.h"

static void list_test_basic_itr_operation(iobject* list) {
	ilist_clear(list);
	dbg_assert(ilist_size(list) == 0);

	ilist_add_front(list, (void*)(intptr_t)4);
	ilist_add_back (list, (void*)(intptr_t)5);
	ilist_add_front(list, (void*)(intptr_t)3);
	ilist_add_back (list, (void*)(intptr_t)6);
	ilist_add_front(list, (void*)(intptr_t)2);
	ilist_add_back (list, (void*)(intptr_t)7);
	ilist_add_front(list, (void*)(intptr_t)1);
	ilist_add_back (list, (void*)(intptr_t)8);
	/* now the list contains { 1, 2, 3, 4, 5, 6, 7, 8 } */

	dbg_assert(ilist_size(list) == 8);
	{
		iterator itr = ilist_itr_create(list, itr_begin);
		const iterator end = ilist_itr_end(list);
		intptr_t current = 1;

		/* traverse the list */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)current);
			current ++;
		}

		/* test itr_assign */
		ilist_itr_assign(list, itr, itr_begin);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)1);
		ilist_itr_assign(list, itr, itr_end);
		itr_to_prev(itr);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)8);

		/* test itr_find */
		ilist_itr_find(list, itr, (void*)(intptr_t)0);
		dbg_assert(itr_equals(itr, ilist_itr_end(list)));
		ilist_itr_find(list, itr, (void*)(intptr_t)9);
		dbg_assert(itr_equals(itr, ilist_itr_end(list)));

		for (current = 1; current <= 8; current ++) {
			ilist_itr_find(list, itr, (void*)current);
			dbg_assert(!itr_equals(itr, ilist_itr_end(list)));
		}

		/* test itr_remove */
		ilist_itr_find(list, itr, (void*)(intptr_t)4);
		dbg_assert(!itr_equals(itr, ilist_itr_end(list)));

		current = (intptr_t)ilist_itr_remove(list, itr);
		/* now the list is { 1, 2, 3,  , 5, 6, 7, 8 } */
		dbg_assert(current == 4);
		dbg_assert(ilist_size(list) == 7);
		for (current = 0; current <= 9; current ++) {
			ilist_itr_find(list, itr, (void*)current);
			if (current == 0 || current == 9 || current == 4) {
				dbg_assert(itr_equals(itr, ilist_itr_end(list)));
			}
			else {
				dbg_assert(!itr_equals(itr, ilist_itr_end(list)));
			}
		}

		/* test continous itr_remove */
		ilist_itr_find(list, itr, (void*)(intptr_t)5);
		dbg_assert(!itr_equals(itr, ilist_itr_end(list)));

		current = (intptr_t)ilist_itr_remove(list, itr);
		/* now the list is { 1, 2, 3,  , 6, 7, 8 } */
		dbg_assert(current == 5);
		dbg_assert(ilist_size(list) == 6);
		for (current = 0; current <= 9; current ++) {
			ilist_itr_find(list, itr, (void*)current);
			if (current == 0 || current == 9 || current == 4 || current == 5) {
				dbg_assert(itr_equals(itr, ilist_itr_end(list)));
			}
			else {
				dbg_assert(!itr_equals(itr, ilist_itr_end(list)));
			}
		}

		/* test insert_before */
		ilist_itr_find(list, itr, (void*)(intptr_t)6);
		dbg_assert(!itr_equals(itr, ilist_itr_end(list)));

		ilist_itr_insert_before(list, itr, (void*)(intptr_t)5);
		/* now the list is { 1, 2, 3,  5, 6, 7, 8 } */
		for (current = 1, ilist_itr_assign(list, itr, itr_begin); 
				!itr_equals(itr, end); 
				current ++) {
			if (current != 4) {
				dbg_assert(itr_get_ref(itr) == (void*)current);
				itr_to_next(itr);
			}
		}

		/* test insert_after */
		ilist_itr_find(list, itr, (void*)(intptr_t)3);
		dbg_assert(!itr_equals(itr, ilist_itr_end(list)));

		ilist_itr_insert_after(list, itr, (void*)(intptr_t)4);
		/* now the list is { 1, 2, 3, 4, 5, 6, 7, 8 } */
		for (current = 1, ilist_itr_assign(list, itr, itr_begin); 
				!itr_equals(itr, end); 
				itr_to_next(itr), current ++) {
			dbg_assert(itr_get_ref(itr) == (void*)current);
		}

		itr_destroy(itr);
	}

	return;
}

static void list_test_basic_operation(iobject* list) {
	ilist_clear(list);
	dbg_assert(ilist_size(list) == 0);

	ilist_add_front(list, (void*)(intptr_t)4);
	ilist_add_back (list, (void*)(intptr_t)5);
	ilist_add_front(list, (void*)(intptr_t)3);
	ilist_add_back (list, (void*)(intptr_t)6);
	ilist_add_front(list, (void*)(intptr_t)2);
	ilist_add_back (list, (void*)(intptr_t)7);
	ilist_add_front(list, (void*)(intptr_t)1);
	ilist_add_back (list, (void*)(intptr_t)8);
	/* now the list contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
	dbg_assert(ilist_size(list) == 8);
	{
		intptr_t current = 1;

		for (; current <= 8; current ++) {
			dbg_assert(ilist_contains(list, (void*)current));
		}
		dbg_assert(ilist_contains(list, (void*)(intptr_t)0) == false);
		dbg_assert(ilist_contains(list, (void*)(intptr_t)9) == false);
	}

	/* test remove, delete the even ones */
	dbg_assert(ilist_size(list) == 8);
	{
		/* now the list contains { 1, 2, 3, 4, 5, 6, 7, 8 } */
		intptr_t x;
		bool res;
		for (x = 2; x <= 8; x +=2) {
			res = ilist_remove(list, (void*)x);
			dbg_assert(res == 1);
		}

		dbg_assert(ilist_size(list) == 4);
		for (x = 1; x <= 8; x ++) {
			bool exist = x % 2 ? true : false;
			dbg_assert(ilist_contains(list, (void*)x) == exist);
		}

		dbg_assert(ilist_contains(list, (void*)(intptr_t)0) == false);
		dbg_assert(ilist_contains(list, (void*)(intptr_t)9) == false);
	}

	/* test remove_front and remove_back */
	dbg_assert(ilist_size(list) == 4);
	{
		/* now the list is {1, 3, 5, 7} */
		intptr_t x;
		x = (intptr_t)ilist_remove_front(list);
		dbg_assert(x == 1);
		dbg_assert(ilist_size(list) == 3);

		x = (intptr_t)ilist_remove_back(list);
		dbg_assert(x == 7);
		dbg_assert(ilist_size(list) == 2);

		x = (intptr_t)ilist_remove_front(list);
		dbg_assert(x == 3);
		dbg_assert(ilist_size(list) == 1);

		x = (intptr_t)ilist_remove_back(list);
		dbg_assert(x == 5);
		dbg_assert(ilist_size(list) == 0);
	}

	/* test clear */
	{
		/* now the list is empty */
		dbg_assert(ilist_size(list) == 0);
		
		/* clear an empty list */
		ilist_clear(list);
		dbg_assert(ilist_size(list) == 0);

		ilist_add_back(list, (void*)(intptr_t)1);
		ilist_add_back(list, (void*)(intptr_t)1);
		ilist_add_back(list, (void*)(intptr_t)1);

		/* now the list is { 1, 1, 1 } */
		ilist_clear(list);
		dbg_assert(ilist_size(list) == 0);
	}
}

void list_test_basic(iobject* list) {

	list_test_basic_operation(list);

	list_test_basic_itr_operation(list);
}

