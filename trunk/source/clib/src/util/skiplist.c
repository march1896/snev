#include <list_link.h>
#include <skiplist.h>

struct skip_link {
	/* because the skip link is vary sized, so the skip link could not be allocated with 
	 * the 'owner', like tree links. So the owner could not be retrieved by the relative 
	 * address(containerof), so, we have to keep a pointer back to the 'owner' */
	const void*      reference;

	int              num_level;
	struct list_link levels[0];
};


struct skiplist {
	struct skip_link* sentinel;

	pf_alloc          __alloc;
	pf_dealloc        __dealloc;
	void*             __heap;

	pf_skip_compare   __comp;
};

struct skip_link* skip_link_create(const void* owner, pf_alloc alc, void* heap);
struct skip_link* skip_link_create_fixed(const void* owner, pf_alloc alc, void* heap, int level);
void skip_link_destroy(struct skip_link* link, pf_dealloc dlc, void* heap);

void skip_link_insert(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);
bool skip_link_insert_s(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);
void skip_link_remove(struct skip_link* header, struct skip_link* target, pf_skip_compare comp);

struct skip_link* skip_link_search(struct skip_link* header, pf_skip_compare comp, void* param);

void skip_link_debug_check(struct skip_link* root, pf_skip_compare comp);

#define SKIP_LINK_MAX_LEVEL 32
struct skip_link* skip_link_create(const void* owner, pf_alloc alc, void* heap) {
	int r = rand();
	int level = 1;
	struct skip_link* link = NULL;

	while (r & 1 && level < SKIP_LINK_MAX_LEVEL) {
		r >>= 1;
		level ++;
	}

	link = (struct skip_link*)alloc(alc, heap, 
			sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	dbg_assert(sizeof(struct skip_link) == offsetof(struct skip_link, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

struct skip_link* skip_link_create_fixed(const void* owner, pf_alloc alc, void* heap, int level) {
	struct skip_link* link = NULL;

	link = (struct skip_link*)alloc(alc, heap, 
			sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	dbg_assert(sizeof(struct skip_link) == offsetof(struct skip_link, levels));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->reference = owner;
	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

void skip_link_destroy(struct skip_link* link, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, link);
}

#define SKIPLINK_FROM_LISTLINK(list, level) container_of(((struct list_link*)list - level), struct skip_link, levels)

void skip_link_insert(struct skip_link* header, struct skip_link* target, pf_skip_compare comp) {
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

/* NOTE: remember to delete the skip_link when failed */
bool skip_link_insert_s(struct skip_link* header, struct skip_link* target, pf_skip_compare comp) {
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
				return false;
			}
			list = list->next;
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[i], list, list->next);
		list --;
		list = list->next;
	}
	return true;
}

void skip_link_remove(struct skip_link* header, struct skip_link* target, pf_skip_compare comp) {
	int i;

	unused(header);
	unused(comp);

	for (i = 0; i < target->num_level; i ++) {
		list_unlink(&target->levels[i]);
	}
}

struct skip_link* skip_link_search(struct skip_link* header, pf_skip_compare comp, void* param) {
	int i;
	struct list_link* list = header->levels[header->num_level-1].next;
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			struct skip_link* skiplink = SKIPLINK_FROM_LISTLINK(list, i);
			int compr = comp(skiplink->reference, param);
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

	return NULL;
}

struct skiplist* skiplist_create(pf_skip_compare comp, pf_alloc alc, pf_dealloc dlc, void* pheap) {
	struct skiplist* thelist = (struct skiplist*)alloc(alc, pheap, sizeof(struct skiplist));
	
	dbg_assert(thelist != NULL);
	thelist->__alloc = alc;
	thelist->__dealloc = dlc;
	thelist->__heap = pheap;
	thelist->__comp = comp;

	thelist->sentinel = skip_link_create_fixed(NULL, alc, pheap, SKIP_LINK_MAX_LEVEL);

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
		skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);
	}

	{
		int level;
		for (level = 0; level < slist->sentinel->num_level; level ++) {
			list_init(&slist->sentinel->levels[level]);
		}
	}
}

void skiplist_destroy(struct skiplist* slist) {
	dbg_assert(slist != NULL && slist->sentinel != NULL);

	skiplist_clear(slist);

	skip_link_destroy(slist->sentinel, slist->__dealloc, slist->__heap);
	dealloc(slist->__dealloc, slist->__heap, slist);
}

void skiplist_insert(struct skiplist* slist, void* data) {
	struct skip_link* skiplink = skip_link_create(data, slist->__alloc, slist->__heap);

	skip_link_insert(slist->sentinel, skiplink, slist->__comp);
}

bool skiplist_insert_s(struct skiplist* slist, void* data) {
	struct skip_link* skiplink = skip_link_create(data, slist->__alloc, slist->__heap);

	bool res = skip_link_insert_s(slist->sentinel, skiplink, slist->__comp);

	if (res == false) {
		skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);
	}
	return res;
}

bool skiplist_contains(struct skiplist* slist, void* data) {
	return skip_link_search(slist->sentinel, slist->__comp, data) != NULL;
}

struct skip_link* skiplist_search(struct skiplist* slist, void* data) {
	return skip_link_search(slist->sentinel, slist->__comp, data);
}

bool skiplist_remove(struct skiplist* slist, void* data) {
	struct skip_link* skiplink = skip_link_search(slist->sentinel, slist->__comp, data);

	if (skiplink == NULL) 
		return false;

	skip_link_remove(slist->sentinel, skiplink, slist->__comp);
	skip_link_destroy(skiplink, slist->__dealloc, slist->__heap);

	return true;
}
