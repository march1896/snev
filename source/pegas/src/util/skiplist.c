#include <util/list_link.h>
#include <util/skiplist.h>
#include <memheap/heap_global.h>

struct skiplink {
	/* because the skip link is vary sized, so the skip link could not be allocated with 
	 * the 'owner', like tree links. So the owner could not be retrieved by the relative 
	 * address(container_of), so, we have to keep a pointer back to the 'owner' */
	const void*      reference;

	int              num_level;
	struct listlink levels[0];
};

static struct skiplink* skip_link_create      (const void* owner, pf_alloc __alloc, void* __heap);
static struct skiplink* skip_link_create_fixed(const void* owner, int level, pf_alloc alc, void* alloc_param);
static void              skip_link_destroy     (struct skiplink* link, pf_dealloc dlc, void* dealloc_param);

static void              skip_link_insert      (struct skiplink* header, struct skiplink* target, pf_skiplist_compare comp);
static struct skiplink* skip_link_insert_s    (struct skiplink* header, struct skiplink* target, pf_skiplist_compare comp);
static void              skip_link_insert_v    (struct skiplink* header, struct skiplink* target, pf_skiplist_compare_v compv, void* cp_context);
static struct skiplink* skip_link_insert_sv   (struct skiplink* header, struct skiplink* target, pf_skiplist_compare_v compv, void* cp_context);

static void              skip_link_remove      (struct skiplink* header, struct skiplink* target);

#define SKIP_LINK_MAX_LEVEL 32
struct skiplink* skip_link_create(const void* owner, pf_alloc __alloc, void* __heap) {
	int r = rand();
	int level = 1;
	struct skiplink* link = NULL;

	while (r & 1 && level < SKIP_LINK_MAX_LEVEL) {
		r >>= 1;
		level ++;
	}

	link = (struct skiplink*)alloc(__alloc, __heap, sizeof(struct listlink) * level + offsetof(struct skiplink, levels));

	dbg_assert(sizeof(struct skiplink) == offsetof(struct skiplink, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct listlink) * level + offsetof(struct skiplink, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

struct skiplink* skip_link_create_fixed(const void* owner, int level, pf_alloc __alloc, void* __heap) {
	struct skiplink* link = NULL;

	link = (struct skiplink*)alloc(__alloc, __heap, sizeof(struct listlink) * level + offsetof(struct skiplink, levels));

	dbg_assert(sizeof(struct skiplink) == offsetof(struct skiplink, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct listlink) * level + offsetof(struct skiplink, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

void skip_link_destroy(struct skiplink* link, pf_dealloc __dealloc, void* __heap) {
	dealloc(__dealloc, __heap, link);
}

#define SKIPLINK_FROM_list_link(list, level) container_of(((struct listlink*)list - level), struct skiplink, levels)

inline const struct skiplink* skip_link_prev(const struct skiplink* cur) {
	struct listlink* listlink = cur->levels[0].prev;

	return SKIPLINK_FROM_list_link(listlink, 0);
}

inline const struct skiplink* skip_link_next(const struct skiplink* cur) {
	struct listlink* listlink = cur->levels[0].next;

	return SKIPLINK_FROM_list_link(listlink, 0);
}

inline const void* skip_link_getref(const struct skiplink* slink) {
	return slink->reference;
}

/* insert the target no matter if there is an element in the list which 'equals' to it */
void skip_link_insert(struct skiplink* header, struct skiplink* target, pf_skiplist_compare comp) {
	int i;
	struct listlink* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skiplink* skiplink = SKIPLINK_FROM_list_link(list, i);
			if (comp(skiplink->reference, target->reference) >= 0) 
				break;
			list = list->next;
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[i], list, list->next);
		list --;
		list = list->next;
	}
}

/* NOTE: remember to delete the skip_link when failed 
 * if there is no data equal to target's reference, insert the target and return target.
 * if there is some data equal to target's reference, don't insert target and return the link */
struct skiplink* skip_link_insert_s(struct skiplink* header, struct skiplink* target, pf_skiplist_compare comp) {
	int i;
	struct listlink* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			struct skiplink* skiplink = SKIPLINK_FROM_list_link(list, i);
			compr = comp(skiplink->reference, target->reference);
			if (compr > 0) 
				break;
			else if (compr == 0) {
				/* unlinked the already linked part */
				while (++i < target->num_level) {
					list_unlink(&target->levels[i]);
				}
				return skiplink;
			}
			list = list->next;
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[i], list, list->next);
		list --;
		list = list->next;
	}
	return target;
}

void skip_link_remove(struct skiplink* header, struct skiplink* target) {
	int i;

	unused(header);

	for (i = 0; i < target->num_level; i ++) {
		list_unlink(&target->levels[i]);
	}
}

void skip_link_insert_v(struct skiplink* header, struct skiplink* target, pf_skiplist_compare_v compv, void* cp_context) {
	int i;
	struct listlink* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skiplink* skiplink = SKIPLINK_FROM_list_link(list, i);
			if (compv(skiplink->reference, target->reference, cp_context) >= 0) 
				break;
			list = list->next;
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[i], list, list->next);
		list --;
		list = list->next;
	}
}

/* NOTE: remember to delete the skip_link when failed 
 * if there is no data equal to target's reference, insert the target and return target.
 * if there is some data equal to target's reference, don't insert target and return the link */
struct skiplink* skip_link_insert_sv(struct skiplink* header, struct skiplink* target, pf_skiplist_compare_v compv, void* cp_context) {
	int i;
	struct listlink* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			struct skiplink* skiplink = SKIPLINK_FROM_list_link(list, i);
			compr = compv(skiplink->reference, target->reference, cp_context);
			if (compr > 0) 
				break;
			else if (compr == 0) {
				/* unlinked the already linked part */
				while (++i < target->num_level) {
					list_unlink(&target->levels[i]);
				}
				return skiplink;
			}
			list = list->next;
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[i], list, list->next);
		list --;
		list = list->next;
	}
	return target;
}

struct skiplist {
	struct skiplink*     sentinel;

	pf_alloc              __alloc;
	pf_dealloc            __dealloc;
	void*                 __heap;

	pf_skiplist_compare   __comp;
	pf_skiplist_compare_v __compv;
	void*                 __comp_context;
};

static struct skiplist* skiplist_create_internal(
	pf_skiplist_compare comp, 
	pf_skiplist_compare_v compv, void* cp_context,
	pf_alloc __alloc, pf_dealloc __dealloc, void* __heap) 
{
	struct skiplist* thelist = (struct skiplist*)alloc(__alloc, __heap, sizeof(struct skiplist));

	dbg_assert(thelist != NULL);
	thelist->__alloc         = __alloc;
	thelist->__dealloc       = __dealloc;
	thelist->__heap          = __heap;

	thelist->__comp          = comp;
	thelist->__compv         = compv;
	thelist->__comp_context  = cp_context;
	dbg_assert(comp != NULL || compv != NULL);

	thelist->sentinel = skip_link_create_fixed(NULL, SKIP_LINK_MAX_LEVEL, __alloc, __heap);

	return thelist;
}


struct skiplist* skiplist_create(pf_skiplist_compare comp) {
	return skiplist_create_a(comp, __global_default_alloc, __global_default_dealloc, __global_default_heap);
}

struct skiplist* skiplist_create_a(pf_skiplist_compare comp, pf_alloc __alloc, pf_dealloc __dealloc, void* __heap) {
	return skiplist_create_internal(comp, NULL, NULL, __alloc, __dealloc, __heap);
}

struct skiplist* skiplist_create_v (pf_skiplist_compare_v compv, void* cp_context) {
	return skiplist_create_va(compv, cp_context, __global_default_alloc, __global_default_dealloc, __global_default_heap);
}
struct skiplist* skiplist_create_va(pf_skiplist_compare_v compv, void* cp_context,pf_alloc __alloc, pf_dealloc __dealloc, void* __heap) {
	return skiplist_create_internal(NULL, compv, cp_context, __alloc, __dealloc, __heap);
}

void skiplist_clear(struct skiplist* slist) {
	/* traverse the first level */
	struct listlink* listlink = NULL;
	struct listlink* listsent = NULL;

	dbg_assert(slist != NULL && slist->sentinel != NULL);
	listsent = &slist->sentinel->levels[0];
	listlink = listsent->next;
	while (listlink != listsent) {
		struct skiplink* skiplink = SKIPLINK_FROM_list_link(listlink, 0);
		listlink = listlink->next;
		skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);
	}

	/* the sentinel node's should be reset */
	{
		int level;
		for (level = 0; level < slist->sentinel->num_level; level ++) {
			list_init(&slist->sentinel->levels[level]);
		}
	}
}

bool skiplist_empty(const struct skiplist* slist) {
	return slist->sentinel == skip_link_next(slist->sentinel);
}

void skiplist_foreach(struct skiplist* slist, pf_skiplist_ref_visit process, void* context) {
	/* traverse the first level */
	struct listlink* listlink = NULL;
	struct listlink* listsent = NULL;

	dbg_assert(slist != NULL && slist->sentinel != NULL);

	listsent = &slist->sentinel->levels[0];
	listlink = listsent->next;
	while (listlink != listsent) {
		struct skiplink* skiplink = SKIPLINK_FROM_list_link(listlink, 0);
		listlink = listlink->next;

		process(skiplink->reference, context);
	}
}

void skiplist_destroy(struct skiplist* slist) {
	dbg_assert(slist != NULL && slist->sentinel != NULL);

	skiplist_clear(slist);

	skip_link_destroy(slist->sentinel, slist->__dealloc, slist->__heap);
	dealloc(slist->__dealloc, slist->__heap, slist);
}

void skiplist_insert(struct skiplist* slist, const void* data) {
	struct skiplink* skiplink = skip_link_create(data, slist->__alloc, slist->__heap);

	if (slist->__comp != NULL) {
		skip_link_insert(slist->sentinel, skiplink, slist->__comp);
	} else {
		dbg_assert(slist->__compv != NULL);
		skip_link_insert_v(slist->sentinel, skiplink, slist->__compv, slist->__comp_context);
	}
}

void* skiplist_replace_s(struct skiplist* slist, const void* data) {
	struct skiplink* toinsert = skip_link_create(data, slist->__alloc, slist->__heap);
	struct skiplink* inlist   = NULL;
	
	if (slist->__comp != NULL) {
		inlist = skip_link_insert_s(slist->sentinel, toinsert, slist->__comp);
	} else {
		dbg_assert(slist->__compv != NULL);
		inlist = skip_link_insert_sv(slist->sentinel, toinsert, slist->__compv, slist->__comp_context);
	}

	if (inlist != toinsert) {
		/* there is duplicate data in the skiplist, replace it and destroy the link we created before*/
		const void* old_reference = inlist->reference; 
		inlist->reference = data;

		skip_link_destroy(toinsert, slist->__dealloc, slist->__heap);

		return (void*)old_reference;
	}
	return NULL;
}

bool skiplist_insert_s(struct skiplist* slist, const void* data) {
	struct skiplink* toinsert = skip_link_create(data, slist->__alloc, slist->__heap);
	struct skiplink* inlist   = NULL;

	if (slist->__comp != NULL) {
		inlist = skip_link_insert_s(slist->sentinel, toinsert, slist->__comp);
	} else {
		dbg_assert(slist->__compv != NULL);
		inlist = skip_link_insert_sv(slist->sentinel, toinsert, slist->__compv, slist->__comp_context);
	}

	if (inlist != toinsert) {
		skip_link_destroy(toinsert, slist->__dealloc, slist->__heap);
		return false;
	}
	return true;
}

bool skiplist_contains(const struct skiplist* slist, const void* data) {
	return skiplist_search(slist, data) != slist->sentinel;
}

const struct skiplink* skiplist_search(const struct skiplist* slist, const void* data) {
	int i;
	struct skiplink* header = slist->sentinel;
	struct listlink* list = header->levels[header->num_level-1].next;
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skiplink* skiplink = SKIPLINK_FROM_list_link(list, i);
			int compr = 0;
			if (slist->__comp != NULL) {
				compr = slist->__comp(skiplink->reference, data);
			} else {
				compr = slist->__compv(skiplink->reference, data, slist->__comp_context);
			}
			if (compr > 0) 
				break;
			else if (compr == 0) {
				return skiplink;
			}
			list = list->next;
		}
		list = list->prev;
		list --;
		list = list->next;
	}

	return slist->sentinel;
}

/* max_smaller is previous to min_greaterorequal,
 * max_smallerorequal is previous to min_greater. 
 * so the algorithm only cares about min_greater and min_greaterorequal. */
const struct skiplink* skiplist_search_v(const struct skiplist* slist, const void* data, enum skiplist_search_option option) {
	int i;
	struct skiplink* header = slist->sentinel;
	struct listlink* list = header->levels[header->num_level-1].next;
	struct skiplink* candidate = header;

	dbg_assert(option < skiplist_search_count);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skiplink* cur = SKIPLINK_FROM_list_link(list, i);
			int compr = 0;
			if (slist->__comp != NULL) {
				compr = slist->__comp(cur->reference, data);
			} else {
				compr = slist->__compv(cur->reference, data, slist->__comp_context);
			}
			if (compr > 0) {
				candidate = cur;
				break;
			} else if (compr == 0) {
				if (option == skiplist_equal) {
					return cur;
				} else if (option == skiplist_min_greater || option == skiplist_max_smallerorequal) {
					/* do nothing, iterate until find the minimum greater one */
				} else if (option == skiplist_min_greaterorequal || option == skiplist_max_smaller) {
					/* break, iterator contains the minimum greater or equal one, after the last
					 * iteration, candidate will contain the first greater or equal one */
					candidate = cur;
					break;
				}
			}
			list = list->next;
		}
		list = list->prev;
		list --;
		list = list->next;
	}

	if (option == skiplist_equal) {
		/* not found, the candidate should be never set */
		dbg_assert(candidate == header);
		return candidate;
	}

	if (option == skiplist_min_greater || option == skiplist_min_greaterorequal) {
		return candidate;
	} 

	/* now we are trying to find max_greater* one */
	return skip_link_prev(candidate);
}

inline const struct skiplink* skiplist_first(const struct skiplist* slist) {
	return SKIPLINK_FROM_list_link(slist->sentinel->levels[0].next, 0);
}

inline const struct skiplink* skiplist_last(const struct skiplist* slist) {
	return SKIPLINK_FROM_list_link(slist->sentinel->levels[0].prev, 0);
}

inline const struct skiplink* skiplist_sent(const struct skiplist* slist) {
	return slist->sentinel;
}

bool skiplist_remove(struct skiplist* slist, const void* data) {
	struct skiplink* skiplink = (struct skiplink*)skiplist_search(slist, data);

	if (skiplink == slist->sentinel) 
		return false;

	skip_link_remove(slist->sentinel, skiplink);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);

	return true;
}

void skiplist_remove_link(struct skiplist* slist, struct skiplink* skiplink) {
	dbg_assert(skiplink != slist->sentinel);

	skip_link_remove(slist->sentinel, skiplink);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);
}
