#ifndef _SKIP_LINK_H_
#define _SKIP_LINK_H_

#include <cominc.h>
#include <heap_def.h>

typedef int (*pf_skip_compare)(const void* ref_a, const void* ref_b);

struct skiplist* skiplist_create(pf_skip_compare comp, pf_alloc alc, pf_dealloc dlc, void* pheap);
void skiplist_destroy(struct skiplist* list);

void skiplist_insert(struct skiplist* list, void* data);
bool skiplist_insert_s(struct skiplist* list, void* data);
bool skiplist_contains(struct skiplist* list, void* data);
struct skip_link* skiplist_search(struct skiplist* list, void* data);
bool skiplist_remove(struct skiplist* list, void* data);

void skiplist_clear(struct skiplist* list);

#endif /* _SKIP_LINK_H_ */

