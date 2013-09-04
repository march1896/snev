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

typedef int (*pf_skip_compare)(const struct skip_link* a, const struct skip_link* b);
void skiplist_insert(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);
bool skiplist_insert_s(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);
void skiplist_remove(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);

typedef int (*pf_skip_direct)(const struct skip_link* cur, void* param);
struct skip_link* skiplist_search(struct skip_link* header, pf_skip_direct direct, void* param);

void              skiplist_debug_check(struct skip_link* root, pf_skip_compare comp);

#endif /* _SKIP_LINK_H_ */

