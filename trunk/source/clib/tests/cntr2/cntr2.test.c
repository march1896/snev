#include <stdint.h>

#include "cntr2/ifactory.h"
#include "cntr2/ilist.h"
#include "cntr2/iqueue.h"
#include "cntr2/istack.h"
#include "cntr2/iset.h"
#include "cntr2/iitr.h"
#include "cntr2/ialgo.h"

#include "memheap/heap_global.h"

#include "test_util.h"
#include "cntr2.test.h"

static void __ref_print(void* __ref) {
	log_printf("%d ", (int)(intptr_t)__ref);
}

void icntr_print(iobject* icntr) {
	ilist list = as_list(icntr);
	iterator begin = ilist_itr_begin(list);
	iterator end   = ilist_itr_end(list);
	log_printtab();

	while (!itr_equals(begin, end)) {
		__ref_print((void*)itr_get_ref(begin));
		itr_to_next(begin);
	}
	foreach(begin, end, __ref_print);

	log_printf("\n");
	itr_destroy(begin);
	itr_destroy(end);
}

static void list_test_base(iobject* list) {
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
		iterator begin = ilist_itr_begin(list);
		iterator end   = ilist_itr_end(list);
		int current = 1;

		for (; current <= 8; current ++) {
			dbg_assert(ilist_contains(list, (void*)(intptr_t)current));
		}
		dbg_assert(ilist_contains(list, (void*)(intptr_t)0) == false);
		dbg_assert(ilist_contains(list, (void*)(intptr_t)9) == false);

		current = 1;
		for (; !itr_equals(begin, end); itr_to_next(begin)) {
			dbg_assert(itr_get_ref(begin) == (void*)(intptr_t)current);
			current ++;
		}

		itr_destroy(begin);
		itr_destroy(end);
	}

	/* now delete the even ones */

	ilist_clear(list);
	dbg_assert(ilist_size(list) == 0);

	{
		/* make sure there is no data stored in the list */
		iterator begin = ilist_itr_begin(list);
		iterator end   = ilist_itr_end(list);

		itr_to_next(begin);
		dbg_assert(itr_equals(begin, end));

		itr_destroy(begin);
		itr_destroy(end);
	}
}

static void list_test_addfindremove(iobject* list, int data_size) {
	void** raw_data = 0;
	int i;

	raw_data = (void**)salloc(sizeof(void*) * data_size);

	for (i = 0; i < data_size; i ++) {
		raw_data[i] = (void*)(intptr_t)i;
	}
	ilist_clear(list);
	dbg_assert(ilist_size(list) == 0);

	for (i = 0; i < data_size; i ++) {
		ilist_add_back(list, raw_data[i]);
	}

	for (i = 0; i < data_size; i ++) {

	}


	sfree(raw_data);
}

void list_correct(iobject* list) {
	void* a[10];
    int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}

	icntr_print(list);           /* { } */
	ilist_add_back(list, a[0]);  /* { 0 } */
	icntr_print(list);
	ilist_add_front(list, a[1]); /* { 1, 0 } */
	icntr_print(list);
	ilist_add_back(list, a[2]);  /* { 1, 0, 2 } */
	icntr_print(list);
	ilist_add_front(list, a[3]); /* { 3, 1, 0, 2 } */
	icntr_print(list);
	ilist_add_back(list, a[4]);  /* { 3, 1, 0, 2, 4 } */
	icntr_print(list);
	ilist_add_front(list, a[5]); /* { 5, 3, 1, 0, 2, 4 } */
	icntr_print(list);

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

	icntr_print(list);

}

void queue_correct(iqueue queue) {
	void* a[10];
	int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}

	icntr_print(queue);
	iqueue_push(queue, a[0]); /* { 0 } */
	icntr_print(queue);
 	iqueue_push(queue, a[1]); /* { 0, 1 } */
 	icntr_print(queue);
	iqueue_push(queue, a[2]); /* { 0, 1, 2 } */
	icntr_print(queue);
	iqueue_push(queue, a[3]); /* { 0, 1, 2, 3 } */
	icntr_print(queue);
	iqueue_push(queue, a[4]); /* { 0, 1, 2, 3, 4 } */
	icntr_print(queue);
	iqueue_push(queue, a[5]); /* { 0, 1, 2, 3, 4, 5} */
	icntr_print(queue);

	dbg_assert(iqueue_size(queue) == 6);

	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 1, 2, 3, 4, 5} */
	//icntr_print(queue);
	dbg_assert(__ref == 0);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 2, 3, 4, 5 } */
	//icntr_print(queue);
	dbg_assert(__ref == 1);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 3, 4, 5 } */
	//icntr_print(queue);
	dbg_assert(__ref == 2);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 4, 5 } */
	//icntr_print(queue);
	dbg_assert(__ref == 3);
	__ref = (int)(intptr_t)iqueue_pop(queue); /* { 5 } */
	//icntr_print(queue);
	dbg_assert(__ref == 4);

	dbg_assert(iqueue_size(queue) == 1);
}

void stack_correct(istack stack) {
    void* a[10];
	int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}
	icntr_print(stack);
	istack_push(stack, a[0]); /* { 0 } */
	icntr_print(stack);
	istack_push(stack, a[1]); /* { 0, 1 } */
	icntr_print(stack);
	istack_push(stack, a[2]); /* { 0, 1, 2 } */
	icntr_print(stack);
	istack_push(stack, a[3]); /* { 0, 1, 2, 3 } */
	icntr_print(stack);
	istack_push(stack, a[4]); /* { 0, 1, 2, 3, 4 } */
	icntr_print(stack);
	istack_push(stack, a[5]); /* { 0, 1, 2, 3, 4, 5} */
	icntr_print(stack);

	dbg_assert(istack_size(stack) == 6);

	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1, 2, 3, 4} */
	icntr_print(stack);
	dbg_assert(__ref == 5);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1, 2, 3 } */
	icntr_print(stack);
	dbg_assert(__ref == 4);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1, 2 } */
	icntr_print(stack);
	dbg_assert(__ref == 3);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0, 1 } */
	icntr_print(stack);
	dbg_assert(__ref == 2);
	__ref = (int)(intptr_t)istack_pop(stack); /* { 0 } */
	icntr_print(stack);
	dbg_assert(__ref == 1);

	dbg_assert(istack_size(stack) == 1);
}

void set_correct(iset set) {
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
}
