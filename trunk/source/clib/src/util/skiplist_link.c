#include <skiplist_link.h>

#define MAX_LEVEL 32
struct skip_link* skip_link_create(pf_alloc alc, void* heap) {
	int r = rand();
	int level = 1;
	struct skip_link* link = NULL;

	while (r & 1 && level < MAX_LEVEL) {
		r >>= 1;
		level ++;
	}

	link = (struct skip_link*)alloc(alc, heap, 
			sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	dbg_assert(sizeof(struct skip_link) == sizeof(int));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

struct skip_link* skip_link_create_fixed(pf_alloc alc, void* heap, int level) {
	struct skip_link* link = NULL;

	link = (struct skip_link*)alloc(alc, heap, 
			sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	dbg_assert(sizeof(struct skip_link) == sizeof(int));
	dbg_assert((char*)&link->levels[level] == (char*)link + sizeof(struct list_link) * level + offsetof(struct skip_link, levels));

	link->num_level = level;
	for (level = 0; level < link->num_level; level ++) {
		list_init(&link->levels[level]);
	}

	return link;
}

#define SKIP_LINK_FROM_LIST(list, level) ((struct skip_link*)((int*)((struct list_link*)list - level) - 1))

void skiplist_insert(struct skip_link* header, struct skip_link* target, pf_skip_compare comp) {
	int i;
	struct list_link* list = &header->levels[header->num_level-1];

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			if (comp(SKIP_LINK_FROM_LIST(list, i), target) >= 0) 
				break;
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[target->num_level], list, list->next);
		list --;
	}
}

/* NOTE: remember to delete the skip_link when failed */
bool skiplist_insert_s(struct skip_link* header, struct skip_link* target, pf_skip_compare comp) {
	int i;
	struct list_link* list = &header->levels[header->num_level-1];

	dbg_assert(header != target);
	dbg_assert(header->num_level >= target->num_level);
	
	for (i = header->num_level-1; i >= 0; i --) {
		int compr = 0;
		while (list != &header->levels[i]) {
			compr = comp(SKIP_LINK_FROM_LIST(list, i), target);
			if (compr > 0) 
				break;
			else if (compr == 0) {
				/* unlinked the already linked part */
				while (++i < target->num_level) {
					list_unlink(&target->levels[i]);
				}
				return false;
			}
		}
		list = list->prev;
		if (target->num_level > i) 
			list_link(&target->levels[target->num_level], list, list->next);
		list --;
	}
	return true;
}

void skiplist_remove(struct skip_link* header, struct skip_link* target, pf_skip_compare comp) {
	int i;

	unused(header);
	unused(comp);

	for (i = 0; i < target->num_level; i ++) {
		list_unlink(&target->levels[i]);
	}
}

struct skip_link* skiplist_search(struct skip_link* header, pf_skip_direct direct, void* param) {
	int i;
	struct list_link* list = &header->levels[header->num_level-1];
	
	for (i = header->num_level-1; i >= 0; i --) {
		while (list != &header->levels[i]) {
			int dir = direct(SKIP_LINK_FROM_LIST(list, i), param);
			if (dir < 0) 
				break;
			else if (dir == 0) {
				return SKIP_LINK_FROM_LIST(list, i);
			}
		}
		list = list->prev;
		list --;
	}

	return NULL;
}

