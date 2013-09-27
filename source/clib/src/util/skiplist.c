#include <list_link.h>
#include <skiplist.h>
#include <heap_global.h>

struct skip_link {
	/* because the skip link is vary sized, so the skip link could not be allocated with 
	 * the 'owner', like tree links. So the owner could not be retrieved by the relative 
	 * address(container_of), so, we have to keep a pointer back to the 'owner' */
	const void*      reference;

	int              num_level;
	struct list_link levels[0];
};

struct skiplist {
	struct skip_link* sentinel;

	pf_skiplink_alloc   __alloc;
	pf_skiplink_dealloc __dealloc;
	void*               __alloc_context;

	pf_skiplist_compare __comp;
};


static void* skiplink_default_alloc(int size, void* param) {
	unused(param);
	return malloc(size);
}

static bool skiplink_default_dealloc(void* buff, void* param) {
	unused(param);
	return free(buff);
}

struct skip_link* skip_link_create(const void* owner, pf_skiplink_alloc __alloc, void* alloc_param);
struct skip_link* skip_link_create_fixed(const void* owner, int level, pf_skiplink_alloc alc, void* alloc_param);
void skip_link_destroy(struct skip_link* link, pf_skiplink_dealloc dlc, void* dealloc_param);

void skip_link_insert(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp);
struct skip_link* skip_link_insert_s(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp);
void skip_link_remove(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp);

void skip_link_debug_check(struct skip_link* root, pf_skiplist_compare comp);

#define SKIP_LINK_MAX_LEVEL 32
struct skip_link* skip_link_create(const void* owner, pf_skiplink_alloc __alloc, void* alloc_param) {
	int r = rand();
	int level = 1;
	struct skip_link* link = NULL;

	while (r & 1 && level < SKIP_LINK_MAX_LEVEL) {
		r >>= 1;
		level ++;
	}

	link = (struct skip_link*)__alloc(sizeof(struct list_link) * level + offsetof(struct skip_link, levels), alloc_param);

	dbg_assert(sizeof(struct skip_link) == offsetof(struct skip_link, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

struct skip_link* skip_link_create_fixed(const void* owner, int level, pf_skiplink_alloc __alloc, void* alloc_param) {
	struct skip_link* link = NULL;

	link = (struct skip_link*)__alloc(sizeof(struct list_link) * level + offsetof(struct skip_link, levels), alloc_param);

	dbg_assert(sizeof(struct skip_link) == offsetof(struct skip_link, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

void skip_link_destroy(struct skip_link* link, pf_skiplink_dealloc __dealloc, void* dealloc_param) {
	__dealloc(link, dealloc_param);
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
 * if there is not data equal to target's reference, insert the target and return target.
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

void skip_link_remove(struct skip_link* header, struct skip_link* target, pf_skiplist_compare comp) {
	int i;

	unused(header);
	unused(comp);

	for (i = 0; i < target->num_level; i ++) {
		list_unlink(&target->levels[i]);
	}
}

struct skiplist* skiplist_create(pf_skiplist_compare comp) {
	struct skiplist* thelist = (struct skiplist*)skiplink_default_alloc(sizeof(struct skiplist), NULL);
	
	dbg_assert(thelist != NULL);
	thelist->__alloc       = skiplink_default_alloc;
	thelist->__dealloc     = skiplink_default_dealloc;
	thelist->__alloc_context = NULL;
	thelist->__comp        = comp;

	thelist->sentinel = skip_link_create_fixed(NULL, SKIP_LINK_MAX_LEVEL, skiplink_default_alloc, NULL);

	return thelist;
}

struct skiplist* skiplist_create_v(pf_skiplist_compare comp, pf_skiplink_alloc __alloc, pf_skiplink_dealloc __dealloc, void* alloc_context) {
	struct skiplist* thelist = (struct skiplist*)__alloc(sizeof(struct skiplist), alloc_context);

	dbg_assert(thelist != NULL);
	thelist->__alloc         = __alloc;
	thelist->__dealloc       = __dealloc;
	thelist->__alloc_context = alloc_context;
	thelist->__comp          = comp;

	thelist->sentinel = skip_link_create_fixed(NULL, SKIP_LINK_MAX_LEVEL, __alloc, alloc_context);

	return thelist;
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
		skip_link_destroy(skiplink, slist->__dealloc, slist->__alloc_context);
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

void skiplist_foreach(struct skiplist* slist, pf_ref_visit_v process, void* context) {
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

	skip_link_destroy(slist->sentinel, slist->__dealloc, slist->__alloc_context);
	slist->__dealloc(slist, slist->__alloc_context);
}

void skiplist_insert(struct skiplist* slist, const void* data) {
	struct skip_link* skiplink = skip_link_create(data, slist->__alloc, slist->__alloc_context);

	skip_link_insert(slist->sentinel, skiplink, slist->__comp);
}

void* skiplist_insert_s(struct skiplist* slist, const void* data) {
	struct skip_link* toinsert = skip_link_create(data, slist->__alloc, slist->__alloc_context);
	struct skip_link* inlist = skip_link_insert_s(slist->sentinel, toinsert, slist->__comp);

	if (inlist != toinsert) {
		/* there is duplicate data in the skiplist, replace it and destroy the link we created before*/
		const void* old_reference = inlist->reference; 
		inlist->reference = data;

		skip_link_destroy(toinsert, slist->__dealloc, slist->__alloc_context);

		return (void*)old_reference;
	}
	return NULL;
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
			int compr = slist->__comp(skiplink->reference, data);
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
			int compr = slist->__comp(cur->reference, data);
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

	skip_link_remove(slist->sentinel, skiplink, slist->__comp);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__alloc_context);

	return true;
}

void skiplist_remove_link(struct skiplist* slist, struct skip_link* skiplink) {
	dbg_assert(skiplink != slist->sentinel);

	skip_link_remove(slist->sentinel, skiplink, slist->__comp);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__alloc_context);
}
