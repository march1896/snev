#ifndef _SKIP_LINK_H_
#define _SKIP_LINK_H_

#include <cominc.h>
#include <heap_def.h>
#include <list_link.h>

struct skip_link {
	int              num_level;
	struct list_link levels[0];
};

struct skip_link* skip_link_create(pf_alloc alc, void* heap);
struct skip_link* skip_link_create_fixed(pf_alloc alc, void* heap, int level);
struct skip_link* skip_link_destroy(struct skip_link* link, pf_dealloc dlc, void* heap);

typedef int (*pf_skip_compare)(const struct skip_link* a, const struct skip_link* b);
void skip_link_insert(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);
bool skip_link_insert_s(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);
void skip_link_remove(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);

typedef int (*pf_skip_direct)(const struct skip_link* cur, void* param);
struct skip_link* skip_link_search(struct skip_link* header, pf_skip_direct direct, void* param);

void              skip_link_debug_check(struct skip_link* root, pf_skip_compare comp);

/*
struct data_t {
	int a;
	int b;
	struct skip_link* link;
	int c;
};

int skip_compare(const struct skip_link* first, const struct skip_link* second) {
	struct data_t* first_d = container_of(first, struct data_t, link);
	struct data_t* second_d = container_of(second, struct data_t, link);

	if (first_d->a != second_d->a) 
		return first_d->a < second_d->a ? -1 : 1;
	if (first_d->b != second_d->b) 
		return first_d->b < second_d->b ? -1 : 1;
	if (first_d->c != second_d->c) 
		return first_d->c < second_d->c ? -1 : 1;

	return 0;
}

{
	skip_link* header = skip_link_create_fixed(heap_buddy_alloc, __global_heap_buddy, 32);

	struct data_t d0;
	init(&d0);
	d0.link = skip_link_create(heap_buddy_alloc, __global_heap_buddy);
	skip_link_insert(header, d0.link, skip_compare);

	init(&d1);
	d1.link = skip_link_create(heap_buddy_alloc, __global_heap_buddy);
	skip_link_insert(header, d1.link, skip_compare);

	skip_link_remove(header, d0.link, skip_compare);
	skip_link_destroy(d0.link, heap_buddy_dealloc, __global_heap_buddy);

	skip_link_destroy(header, heap_buddy_alloc, __global_heap_buddy);
}
*/

/* not visible in .h */
typedef bool (*pf_skiplist_setdata)(struct skip_link* link, void* data);
typedef bool (*pf_skiplist_getdata)(struct skip_link* link, const void* data);
typedef int  (*pf_skiplist_direct) (struct skip_link* link, const void* data);

struct skiplist {
	struct skip_link* sentinel;

	pf_alloc          __alloc;
	pf_dealloc        __dealloc;
	void*             __heap;

	pf_skiplist_setdata setdata;
	pf_skiplist_getdata getdata;
	pf_skiplist_direct  direct;
};

struct skiplist* skiplist_create(
		pf_skiplist_setdata setter, 
		pf_skiplist_getdata getter, 
		pf_skiplist_direct director, 
		pf_alloc alc, 
		pf_dealloc dlc, 
		void* pheap);

void skiplist_insert(struct skiplist* list, void* data);
bool skiplist_insert_s(struct skiplist* list, void* data);
bool skiplist_contains(struct skiplist* list, void* data);
bool skiplist_remove(struct skiplist* list, void* data);

#endif /* _SKIP_LINK_H_ */

