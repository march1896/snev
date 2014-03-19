#ifndef _SKIP_LINK_H_
#define _SKIP_LINK_H_

#include <util/cominc.h>
#include <memheap/heap_def.h>

extern inline const struct skiplink* skip_link_next(const struct skiplink* cur);
extern inline const struct skiplink* skip_link_prev(const struct skiplink* cur);
extern inline const void*            skip_link_getref(const struct skiplink* slink);

typedef int (*pf_skiplist_compare)(const void* ref_a, const void* ref_b);
typedef int (*pf_skiplist_compare_v)(const void* ref_a, const void* ref_b, void* context);

struct skiplist* skiplist_create     (pf_skiplist_compare comp);
struct skiplist* skiplist_create_a   (pf_skiplist_compare comp, pf_alloc alc, pf_dealloc dlc, void* heap);
struct skiplist* skiplist_create_v   (pf_skiplist_compare_v compv, void* cp_context);
struct skiplist* skiplist_create_va  (pf_skiplist_compare_v compv, void* cp_context, pf_alloc alc, pf_dealloc dlc, void* heap);
void             skiplist_destroy    (struct skiplist* list);

void             skiplist_insert     (struct skiplist* list, const void* data);
/* if there is an element in the skiplist 'equals' data, insert_s will do nothing and return false, if not, insert the element */
bool             skiplist_insert_s   (struct skiplist* list, const void* data);
/* if there is an element in the skiplist 'equals' data, update_s will update the reference to data and return the old reference.
   if not, return NULL */
void*            skiplist_replace_s  (struct skiplist* list, const void* data);
bool             skiplist_contains   (const struct skiplist* list, const void* data);
bool             skiplist_remove     (struct skiplist* list, const void* data);
void             skiplist_remove_link(struct skiplist* list, struct skiplink* link);
void             skiplist_clear      (struct skiplist* list);
bool             skiplist_empty      (const struct skiplist* list);

extern inline const struct skiplink* skiplist_first(const struct skiplist* list);
extern inline const struct skiplink* skiplist_last (const struct skiplist* list);
extern inline const struct skiplink* skiplist_sent (const struct skiplist* list);
const struct skiplink* skiplist_search(const struct skiplist* list, const void* data);
enum skiplist_search_option {
	skiplist_equal,
	skiplist_min_greater,
	skiplist_min_greaterorequal,
	skiplist_max_smaller,
	skiplist_max_smallerorequal,
	skiplist_search_count
};
/* the result may contains sentinel link */
const struct skiplink* skiplist_search_v(const struct skiplist* slist, const void* data, enum skiplist_search_option option);

typedef void (*pf_skiplist_ref_visit)(const void* __ref, void* context);
/* note, if you change the inner value of ref, it may destroy the skiplist order */
void skiplist_foreach(struct skiplist* list, pf_skiplist_ref_visit process, void* context);

#endif /* _SKIP_LINK_H_ */

