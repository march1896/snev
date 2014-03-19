#include <list_link.h>
#include <skiplist.h>
#include <heap_global.h>

/* we always ops on skip_link instead of the structure it points to */
typedef struct skip_struct* skiplink;
struct skip_struct {
	/* because the skip link is vary sized, so the skip link could not be allocated with 
	 * the 'owner', like tree links. So the owner could not be retrieved by the relative 
	 * address(container_of), so, we have to keep a pointer back to the 'owner' */
	skiplink*        owner;

	int              num_level;
	struct list_link levels[0];
};

static void skiplink_init      (skiplink link, pf_alloc __alloc, void* __heap);
static void skiplink_init_sent (skiplink link, pf_alloc __alloc, void* __heap);
static void skiplink_deinit    (skiplink link, pf_dealloc __dealloc, void* __heap);

static void skiplink_insert    (skiplink sent, skiplink target, pf_skiplist_compare comp);
static bool skiplink_insert_s  (skiplink sent, skiplink target, pf_skiplist_compare comp);
static void skiplink_insert_v  (skiplink sent, skiplink target, pf_skiplist_compare_v comp, void* comp_context);
static bool skiplink_insert_sv (skiplink sent, skiplink target, pf_skiplist_compare_v comp, void* comp_context);

static void skiplink_remove    (skiplink sent, skiplink target);

#define SKIP_LINK_MAX_LEVEL 32
int skiplink_gen_level() {
	int r = rand();
	int level = 1;

	while (r & 1 && level < SKIP_LINK_MAX_LEVEL) {
		r >>= 1;
		level ++;
	}

	return level;
}

static inline void skiplink_init_internal(skiplink link, int level, pf_alloc __alloc, void* __heap) {
	struct skip_struct* __link = (struct skip_struct*)
		alloc(__alloc, __heap, sizeof(struct list_link)*level + offsetof(struct skip_struct, levels));

	dbg_assert(sizeof(struct skip_struct) == offsetof(struct skip_struct, levels));
	dbg_assert((char*)&__link->levels[level] == (char*)__link + sizeof(struct list_link)*level + offsetof(struct skip_struct, levels));

	__link->owner = link;
	__link->num_level = level;
	for (level = 0; level < __link->num_level; level ++) {
		list_init(&__link->levels[level]);
	}

	link = __link;
}
static void skiplink_init(skiplink link, pf_alloc __alloc, void* __heap) {
	skiplink_init_internal(link, skiplink_gen_level(), __alloc, __heap);
}
static void skiplink_init_sent (skiplink link, pf_alloc __alloc, void* __heap) {
	skiplink_init_internal(link, SKIP_LINK_MAX_LEVEL, __alloc, __heap);
}

static void skiplink_deinit(skiplink link, pf_dealloc __dealloc, void* __heap);
	dealloc(__dealloc, __heap, link);
}

#define SKIPLINK_FROM_LISTLINK(list, level) container_of(((struct list_link*)list - level), struct skip_struct, levels)

inline skiplink skiplink_getowner(skiplink link) {
	return *(link->owner);
}

inline skiplink skiplink_prev(skiplink cur) {
	struct list_link* listlink = cur->levels[0].prev;

	return skiplink_getowner(SKIPLINK_FROM_LISTLINK(listlink, 0));
}

inline skiplink skiplink_next(skiplink cur) {
	struct list_link* listlink = cur->levels[0].next;

	return skiplink_getowner(SKIPLINK_FROM_LISTLINK(listlink, 0));
}

/* insert the target no matter if there is an element in the list which 'equals' to it */
static bool skiplink_insert_s  (skiplink sent, skiplink target, pf_skiplist_compare comp);
static void skiplink_insert_v  (skiplink sent, skiplink target, pf_skiplist_compare_v comp, void* comp_context);
static bool skiplink_insert_sv (skiplink sent, skiplink target, pf_skiplist_compare_v comp, void* comp_context);
void skiplink_insert(skiplink header, skiplink target, pf_skiplist_compare comp);
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
struct skip_link* skip_link_insert_s(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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

void skip_link_remove(struct skip_link* header, struct skip_link* target) {
	int i;

	unused(header);

	for (i = 0; i < target->num_level; i ++) {
		list_unlink(&target->levels[i]);
	}
}

void skip_link_insert_v(struct skip_link* header, struct skip_link* target, pf_skiplist_compare_v compv, void* cp_context) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
struct skip_link* skip_link_insert_sv(struct skip_link* header, struct skip_link* target, pf_skiplist_compare_v compv, void* cp_context) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
	struct skip_link*     sentinel;

	pf_alloc              __alloc;
	pf_dealloc            __dealloc;
	void*                 __heap;

	pf_skiplist_compare   __comp;
	pf_skiplist_compare_v __compv;
	void*                 __comp_context;
};

static struct skip_link* skip_link_create      (const void* owner, pf_alloc __alloc, void* __heap);
static struct skip_link* skip_link_create_fixed(const void* owner, int level, pf_alloc alc, void* alloc_param);
static void              skip_link_destroy     (struct skip_link* link, pf_dealloc dlc, void* dealloc_param);

static void              skip_link_insert      (struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp);
static struct skip_link* skip_link_insert_s    (struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp);
static void              skip_link_insert_v    (struct skip_link* header, struct skip_link* target, pf_skiplist_compare_v compv, void* cp_context);
static struct skip_link* skip_link_insert_sv   (struct skip_link* header, struct skip_link* target, pf_skiplist_compare_v compv, void* cp_context);

static void              skip_link_remove      (struct skip_link* header, struct skip_link* target);
static void              skip_link_debug_check (struct skip_link* root, pf_skiplist_compare comp);

#define SKIP_LINK_MAX_LEVEL 32
struct skip_link* skip_link_create(const void* owner, pf_alloc __alloc, void* __heap) {
	int r = rand();
	int level = 1;
	struct skip_link* link = NULL;

	while (r & 1 && level < SKIP_LINK_MAX_LEVEL) {
		r >>= 1;
		level ++;
	}

	link = (struct skip_link*)alloc(__alloc, __heap, sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	dbg_assert(sizeof(struct skip_link) == offsetof(struct skip_link, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

struct skip_link* skip_link_create_fixed(const void* owner, int level, pf_alloc __alloc, void* __heap) {
	struct skip_link* link = NULL;

	link = (struct skip_link*)alloc(__alloc, __heap, sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	dbg_assert(sizeof(struct skip_link) == offsetof(struct skip_link, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

void skip_link_destroy(struct skip_link* link, pf_dealloc __dealloc, void* __heap) {
	dealloc(__dealloc, __heap, link);
}

#define SKIPLINK_FROM_LISTLINK(list, level) container_of(((struct list_link*)list - level), struct skip_link, levels)

inline const struct skip_link* skip_link_prev(const struct skip_link* cur) {
	struct list_link* listlink = cur->levels[0].prev;

	return SKIPLINK_FROM_LISTLINK(listlink, 0);
}

inline const struct skip_link* skip_link_next(const struct skip_link* cur) {
	struct list_link* listlink = cur->levels[0].next;

	return SKIPLINK_FROM_LISTLINK(listlink, 0);
}

inline const void* skip_link_getref(const struct skip_link* slink) {
	return slink->reference;
}

/* insert the target no matter if there is an element in the list which 'equals' to it */
void skip_link_insert(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
struct skip_link* skip_link_insert_s(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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

void skip_link_remove(struct skip_link* header, struct skip_link* target) {
	int i;

	unused(header);

	for (i = 0; i < target->num_level; i ++) {
		list_unlink(&target->levels[i]);
	}
}

void skip_link_insert_v(struct skip_link* header, struct skip_link* target, pf_skiplist_compare_v compv, void* cp_context) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
struct skip_link* skip_link_insert_sv(struct skip_link* header, struct skip_link* target, pf_skiplist_compare_v compv, void* cp_context) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
	struct list_link* listlink = NULL;
	struct list_link* listsent = NULL;

	dbg_assert(slist != NULL && slist->sentinel != NULL);
	listsent = &slist->sentinel->levels[0];
	listlink = listsent->next;
	while (listlink != listsent) {
		struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(listlink, 0);
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
	struct list_link* listlink = NULL;
	struct list_link* listsent = NULL;

	dbg_assert(slist != NULL && slist->sentinel != NULL);

	listsent = &slist->sentinel->levels[0];
	listlink = listsent->next;
	while (listlink != listsent) {
		struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(listlink, 0);
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
	struct skip_link* skiplink = skip_link_create(data, slist->__alloc, slist->__heap);

	if (slist->__comp != NULL) {
		skip_link_insert(slist->sentinel, skiplink, slist->__comp);
	} else {
		dbg_assert(slist->__compv != NULL);
		skip_link_insert_v(slist->sentinel, skiplink, slist->__compv, slist->__comp_context);
	}
}

void* skiplist_replace_s(struct skiplist* slist, const void* data) {
	struct skip_link* toinsert = skip_link_create(data, slist->__alloc, slist->__heap);
	struct skip_link* inlist   = NULL;
	
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
	struct skip_link* toinsert = skip_link_create(data, slist->__alloc, slist->__heap);
	struct skip_link* inlist   = NULL;

	if (slist->__comp != NULL) {
		inlist = skip_link_insert_s(slist->sentinel, toinsert, slist->__comp);
	} else {
		dbg_assert(slist->__compv != NULL);
		inlist = skip_link_insert_sv(slist->sentinel, toinsert, slist->__compv, slist->__comp_context);
	}

	if (inlist != toinsert) {
		return false;
	}
	return true;
}

bool skiplist_contains(const struct skiplist* slist, const void* data) {
	return skiplist_search(slist, data) != slist->sentinel;
}

const struct skip_link* skiplist_search(const struct skiplist* slist, const void* data) {
	int i;
	struct skip_link* header = slist->sentinel;
	struct list_link* list = header->levels[header->num_level-1].next;
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
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
const struct skip_link* skiplist_search_v(const struct skiplist* slist, const void* data, enum skiplist_search_option option) {
	int i;
	struct skip_link* header = slist->sentinel;
	struct list_link* list = header->levels[header->num_level-1].next;
	struct skip_link* candidate = header;

	dbg_assert(option < skiplist_search_count);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* cur = SKIPLINK_FROM_LISTLINK(list, i);
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
					/* break, itr contains the minimum greater or equal one, after the last
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

inline const struct skip_link* skiplist_first(const struct skiplist* slist) {
	return SKIPLINK_FROM_LISTLINK(slist->sentinel->levels[0].next, 0);
}

inline const struct skip_link* skiplist_last(const struct skiplist* slist) {
	return SKIPLINK_FROM_LISTLINK(slist->sentinel->levels[0].prev, 0);
}

inline const struct skip_link* skiplist_sent(const struct skiplist* slist) {
	return slist->sentinel;
}

bool skiplist_remove(struct skiplist* slist, const void* data) {
	struct skip_link* skiplink = (struct skip_link*)skiplist_search(slist, data);

	if (skiplink == slist->sentinel) 
		return false;

	skip_link_remove(slist->sentinel, skiplink);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);

	return true;
}

void skiplist_remove_link(struct skiplist* slist, struct skip_link* skiplink) {
	dbg_assert(skiplink != slist->sentinel);

	skip_link_remove(slist->sentinel, skiplink);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);
}
