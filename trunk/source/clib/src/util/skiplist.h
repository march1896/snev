#ifndef _SKIP_LINK_H_
#define _SKIP_LINK_H_

#include <cominc.h>

extern inline const struct skip_link* skip_link_next(const struct skip_link* cur);
extern inline const struct skip_link* skip_link_prev(const struct skip_link* cur);
extern inline const void*             skip_link_getref(const struct skip_link* slink);

typedef int (*pf_skiplist_compare)(const void* ref_a, const void* ref_b);
/* we don't directly use pf_alloc/pf_dealloc, to give the change to use other memory
 * management method like allocator */
typedef void* (*pf_skiplink_alloc)(int size_in_byte, void* param);
typedef bool (*pf_skiplink_dealloc)(void* buff, void* param);

struct skiplist* skiplist_create(pf_skiplist_compare comp);
/* the client should handle the life_time of alloc_context */
struct skiplist* skiplist_create_v(pf_skiplist_compare comp, 
		pf_skiplink_alloc alc, pf_skiplink_dealloc dlc, void* alloc_context);
void   skiplist_destroy(struct skiplist* list);

void skiplist_insert  (struct skiplist* list, void* data);
bool skiplist_insert_s(struct skiplist* list, void* data);
bool skiplist_contains(const struct skiplist* list, void* data);
bool skiplist_remove  (struct skiplist* list, void* data);
void skiplist_remove_link(struct skiplist* list, struct skip_link* link);
void skiplist_clear   (struct skiplist* list);
bool skiplist_empty   (const struct skiplist* list);

extern inline const struct skip_link* skiplist_first(const struct skiplist* list);
extern inline const struct skip_link* skiplist_last (const struct skiplist* list);
extern inline const struct skip_link* skiplist_sent (const struct skiplist* list);
const struct skip_link* skiplist_search(const struct skiplist* list, void* data);
enum skiplist_search_option {
	skiplist_equal,
	skiplist_min_greater,
	skiplist_min_greaterorequal,
	skiplist_max_smaller,
	skiplist_max_smallerorequal,
	skiplist_search_count
};
/* the result may contains sentinel link */
const struct skip_link* skiplist_search_v(const struct skiplist* slist, void* data, enum skiplist_search_option option);

/* note, if you change the inner value of ref, it may destroy the skiplist order */
typedef void (*pf_ref_process)(const void* __ref, void* context);
void skiplist_foreach(struct skiplist* list, pf_ref_process process, void* context);

#endif /* _SKIP_LINK_H_ */

