#include "cntr2/imset.h"
#include "cntr2/iitr.h"
#include "cntr2/ialgo.h"

#include "imset.test.h"
#include "test_util.h"

struct counter_t {
	int count; /* count of element */
	int sum;   /* sum of all element */
};
static void foreach_count(const void* __ref, void* param) {
	struct counter_t* counter = (struct counter_t*)param;

	counter->count ++;
	counter->sum += (intptr_t)__ref;
}

static void mset_test_basic_itr_operation(iobject* mset) {
	imset_clear(mset);
	dbg_assert(imset_empty(mset));

	imset_insert(mset, (void*)(intptr_t)4);
	imset_insert(mset, (void*)(intptr_t)1);
	imset_insert(mset, (void*)(intptr_t)3);
	imset_insert(mset, (void*)(intptr_t)2);
	imset_insert(mset, (void*)(intptr_t)2);
	imset_insert(mset, (void*)(intptr_t)3);
	imset_insert(mset, (void*)(intptr_t)1);
	imset_insert(mset, (void*)(intptr_t)4);
	/* now the mset contains { 1, 1, 2, 2, 3, 3, 4, 4 } */

	dbg_assert(imset_size(mset) == 8);
	{
		iterator itr   = imset_itr_create(mset, itr_begin);
		iterator lower = imset_itr_create(mset, itr_begin);
		iterator upper = imset_itr_create(mset, itr_begin);
		struct counter_t counter;
		const_iterator end = imset_itr_end(mset);
		intptr_t current = 1;

		/* traverse the mset */
		for (; !itr_equals(itr, end); itr_to_next(itr)) {
			dbg_assert(itr_get_ref(itr) == (void*)((current+1)/2));
			current ++;
		}

		/* test itr_assign */
		imset_itr_assign(mset, itr, itr_begin);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)1);
		imset_itr_assign(mset, itr, itr_end);
		itr_to_prev(itr);
		dbg_assert(itr_get_ref(itr) == (void*)(intptr_t)4);

		/* test itr_find_lower, itr_find_upper */
		/* find element not int the mset */
		imset_itr_find_lower(mset, lower, (void*)(intptr_t)0);
		dbg_assert(itr_equals(lower, imset_itr_end(mset)));
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)0);
		dbg_assert(itr_equals(upper, imset_itr_end(mset)));

		imset_itr_find_lower(mset, lower, (void*)(intptr_t)1);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)1);
		dbg_assert(!itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 2);
		dbg_assert(counter.sum == 2);

		imset_itr_find_lower(mset, lower, (void*)(intptr_t)3);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)3);
		dbg_assert(!itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 2);
		dbg_assert(counter.sum == 6);

		imset_itr_find_lower(mset, lower, (void*)(intptr_t)1);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)3);
		dbg_assert(!itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 6);
		dbg_assert(counter.sum == 12); /* 1 + 1 + 2 + 2 + 3 + 3  */

		/* test itr_remove */
		imset_itr_find_lower(mset, itr, (void*)(intptr_t)2);
		dbg_assert(!itr_equals(itr, imset_itr_end(mset)));

		current = (intptr_t)imset_itr_remove(mset, itr);
		/* now the mset is { 1, 1, 2,  3, 3, 4, 4 } */
		dbg_assert(current == 2);
		dbg_assert(imset_size(mset) == 7);
		imset_itr_find_lower(mset, lower, (void*)(intptr_t)4);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)4);
		dbg_assert(!itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 1);
		dbg_assert(counter.sum == 2);

		imset_itr_find_lower(mset, lower, (void*)(intptr_t)1);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)3);
		dbg_assert(!itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 5);
		dbg_assert(counter.sum == 10); /* 1 + 1 + 2 + 3 + 3  */

		/* test continuous itr_remove */
		imset_itr_find_lower(mset, itr, (void*)(intptr_t)2);
		dbg_assert(!itr_equals(itr, imset_itr_end(mset)));

		current = (intptr_t)imset_itr_remove(mset, itr);
		/* now the mset is { 1, 1, 3, 3, 4, 4 } */
		dbg_assert(current == 2);
		dbg_assert(imset_size(mset) == 6);
		imset_itr_find_lower(mset, lower, (void*)(intptr_t)2);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)2);
		dbg_assert(itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 0);
		dbg_assert(counter.sum == 0);

		imset_itr_find_lower(mset, lower, (void*)(intptr_t)1);
		imset_itr_find_upper(mset, upper, (void*)(intptr_t)2);
		dbg_assert(itr_equals(lower, upper));
		counter.count = 0;
		counter.sum = 0;
		foreach_v(lower, upper, foreach_count, &counter);
		dbg_assert(counter.count == 2);
		dbg_assert(counter.sum == 2); /* 1 + 1 */

		itr_destroy(itr);
		itr_destroy(lower);
		itr_destroy(upper);
	}

	return;
}

static void mset_test_basic_operation(iobject* mset) {
	imset_clear(mset);
	dbg_assert(imset_empty(mset));

	{
		imset_insert(mset, (void*)(intptr_t)4);
		imset_insert(mset, (void*)(intptr_t)1);
		imset_insert(mset, (void*)(intptr_t)3);
		imset_insert(mset, (void*)(intptr_t)2);
		imset_insert(mset, (void*)(intptr_t)2);
		imset_insert(mset, (void*)(intptr_t)3);
		imset_insert(mset, (void*)(intptr_t)1);
		imset_insert(mset, (void*)(intptr_t)4);
	}
	/* now the mset contains { 1, 1, 2, 2, 3, 3, 4, 4 } */
	dbg_assert(imset_size(mset) == 8);
	{
		intptr_t current = 1;

		dbg_assert(imset_contains(mset, (void*)(intptr_t)0) == false);
		dbg_assert(imset_contains(mset, (void*)(intptr_t)5) == false);

		for (current = 1; current <= 4; current ++) {
			dbg_assert(imset_contains(mset, (void*)current) == true);
			dbg_assert(imset_count(mset, (void*)current) == 2);
		}
	}

	/* test remove, delete the even ones */
	dbg_assert(imset_size(mset) == 8);
	dbg_assert(imset_empty(mset) == false);
	{
		/* now the mset contains { 1, 1, 2, 2, 3, 3, 4, 4 } */
		intptr_t x;
		bool res;

		res = imset_remove(mset, (void*)(intptr_t)2);
		/* now the mset contains { 1, 1, 2,  3, 3, 4, 4 } */
		dbg_assert(res == true);
		dbg_assert(imset_size(mset) == 7);
		dbg_assert(imset_contains(mset, (void*)(intptr_t)2) == true);
		dbg_assert(imset_count(mset, (void*)(intptr_t)2) == 1);

		res = imset_remove(mset, (void*)(intptr_t)2);
		/* now the mset contains { 1, 1,  3, 3, 4, 4 } */
		dbg_assert(res == true);
		dbg_assert(imset_size(mset) == 6);
		dbg_assert(imset_contains(mset, (void*)(intptr_t)2) == false);
		dbg_assert(imset_count(mset, (void*)(intptr_t)2) == 0);

		/* try to remove element not in mset */
		res = imset_remove(mset, (void*)(intptr_t)2);
		/* now the mset contains { 1, 1,  3, 3, 4, 4 } */
		dbg_assert(res == false);
		dbg_assert(imset_size(mset) == 6);
		dbg_assert(imset_contains(mset, (void*)(intptr_t)2) == false);
		dbg_assert(imset_count(mset, (void*)(intptr_t)2) == 0);

		imset_insert(mset, (void*)(intptr_t)3);
		/* now the mset contains { 1, 1, 3, 3, 3, 4, 4 } */
		dbg_assert(imset_size(mset) == 7);
		dbg_assert(imset_contains(mset, (void*)(intptr_t)2) == false);
		dbg_assert(imset_contains(mset, (void*)(intptr_t)3) == true);
		dbg_assert(imset_count(mset, (void*)(intptr_t)3) == 3);
		
	}

	dbg_assert(imset_size(mset) == 7);

	/* test clear */
	{
		imset_clear(mset);
		/* now the mset is empty */
		dbg_assert(imset_empty(mset));
		
		/* clear an empty mset */
		imset_clear(mset);
		dbg_assert(imset_empty(mset));

		imset_insert(mset, (void*)(intptr_t)1);
		imset_insert(mset, (void*)(intptr_t)1);
		imset_insert(mset, (void*)(intptr_t)1);

		/* now the mset is { 1, 1, 1 } */
		imset_clear(mset);
		dbg_assert(imset_empty(mset));
	}
}

void mset_test_datamset(iobject* mset, int datamset_size) {
	intptr_t x = 0;
	bool res = false;

	imset_clear(mset);
	dbg_assert(imset_empty(mset));

	for (x = 0; x < (intptr_t)datamset_size; x ++) {
		imset_insert(mset, (void*)x);
	}
	dbg_assert(imset_size(mset) == datamset_size);

	/* insert the data mset again */
	for (x = 0; x < (intptr_t)datamset_size; x ++) {
		imset_insert(mset, (void*)x);
	}
	dbg_assert(imset_size(mset) == 2 * datamset_size);

	for (x = 0; x < (intptr_t)datamset_size; x ++) {
		int count;
		res = imset_contains(mset, (void*)x);
		dbg_assert(res == true);
		
		count = imset_count(mset, (void*)x);
		dbg_assert(count == 2);
	}
	dbg_assert(imset_contains(mset, (void*)(x + 1)) == false);

	for (x = 0; x < (intptr_t)datamset_size; x ++) {
		res = imset_remove(mset, (void*)x);
		dbg_assert(res == true);
	}
	dbg_assert(imset_size(mset) == datamset_size);
	for (x = 0; x < (intptr_t)datamset_size; x ++) {
		res = imset_remove(mset, (void*)x);
		dbg_assert(res == true);
	}
	dbg_assert(imset_size(mset) == 0);
}

void mset_test_basic(iobject* mset) {

	mset_test_basic_operation(mset);

	mset_test_basic_itr_operation(mset);

	mset_test_datamset(mset, 100000); 
}

void mset_test_memory(iobject* mset) {
}

void mset_test_bench(iobject* mset, int datamset_size) {
	/* the test bench should contain special data query pattern */
}
