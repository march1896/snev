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
#include "cntr2/test.h"

static void __ref_print(void* __ref) {
	log_printf("%d ", (int)(intptr_t)__ref);
}

void icntr_print(iobject* icntr) {
	ilist list = as_list(icntr);
	/* TODO: why here is no warnings */
	iterator itr = ilist_itr_create(list, itr_begin);
	const iterator end   = ilist_itr_end(list);
	log_printtab();

	while (!itr_equals(itr, end)) {
		__ref_print((void*)itr_get_ref(itr));
		itr_to_next(itr);
	}
	foreach(itr, end, __ref_print);

	log_printf("\n");
	itr_destroy(itr);
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
	iterator itr;
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

	itr = ilist_itr_create(list, itr_begin);
	for (i = 0; i <= 5; i ++) {
		ilist_itr_find(list, itr, a[i]);

		dbg_assert(!itr_equals(itr, ilist_itr_end(list)));
		dbg_assert(itr_get_ref(itr) == a[i]);
	}
	

	for (i = 6; i < 10; i ++) {
		ilist_itr_find(list, itr, a[i]);

		dbg_assert(itr_equals(itr, ilist_itr_end(list)));
	}
	itr_destroy(itr);
	
	dbg_assert(ilist_size(list) == 6);

 	__ref = (int)(intptr_t)ilist_remove_back(list); /* { 3, 1, 0, 2, 4 } */
 	dbg_assert(__ref == 4);
 	__ref = (int)(intptr_t)ilist_remove_back(list); /* { 1, 0, 2, 4 } */
 	dbg_assert(__ref == 2);

	icntr_print(list);
	
	ilist_clear(list);
}

void queue_correct(iqueue queue) {
	void* a[10];
	int i, __ref;
	for (i = 0; i < 10; i ++) {
		a[i] = (void*)(intptr_t)i;
	}

	iqueue_clear(queue);
	dbg_assert(iqueue_size(queue) == 0);

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

	iqueue_clear(queue);
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

	istack_clear(stack);
}

void set_correct(iset set) {
    void* a[10];
	iterator itr;
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

	itr = iset_itr_create(set, itr_begin);
	for (i = 0; i < 10; i ++) {
		iset_itr_find(set, itr, a[i]);

		if (i % 2 == 0) {
			dbg_assert(!itr_equals(itr, iset_itr_end(set)));
			dbg_assert(itr_get_ref(itr) == a[i]);
		}
		else {
			dbg_assert(itr_equals(itr, iset_itr_end(set)));
		}
	}
	itr_destroy(itr);

	iset_clear(set);
}
