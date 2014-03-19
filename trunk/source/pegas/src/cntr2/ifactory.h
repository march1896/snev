#ifndef _INTERFACE_FACTORY_H_
#define _INTERFACE_FACTORY_H_

#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

// #define as_list(obj)  (_interface)__cast((obj), ILIST_ID)
// #define as_queue(obj) (_interface)__cast((obj), IQUEUE_ID)
// #define as_stack(obj) (_interface)__cast((obj), ISTACK_ID)
// #define as_set(obj)   (_interface)__cast((obj), ISET_ID)
// #define as_mset(obj)  (_interface)__cast((obj), IMSET_ID)
// #define as_map(obj)   (_interface)__cast((obj), IMAP_ID)
// #define as_mmap(obj)  (_interface)__cast((obj), IMMAP_ID)

/* if create without allocator, it will use the global_default_allocator
 * if the alc == NULL, the create_a will create with a multi-poll allocator for best efficiency */
_object* cntr_create_oarray(unknown_traits* content_traits);
_object* cntr_create_oarray_a(unknown_traits* content_traits, allocator alc);

_object* cntr_create_olist(unknown_traits* content_traits);
_object* cntr_create_olist_a(unknown_traits* content_traits, allocator alc);

_object* cntr_create_ollrb(unknown_traits* content_traits);
_object* cntr_create_ollrb_a(unknown_traits* content_traits, allocator alc);

_object* cntr_create_osplay(unknown_traits* content_traits);
_object* cntr_create_osplay_a(unknown_traits* content_traits, allocator alc);

_object* cntr_create_oskiplist(unknown_traits* content_traits);
_object* cntr_create_oskiplist_a(unknown_traits* content_traits, allocator alc);

struct pair {
	const void* key;
	void*       value;
};

_object* cntr_create_ordmap(unknown_traits* key_traits, unknown_traits* value_traits);
enum ordmap_driver {
	ordmap_by_llrb,
	ordmap_by_splay,
	ordmap_by_skiplist,
	ordmap_driver_count
};
_object* cntr_create_ordmap_v(unknown_traits* key_traits, unknown_traits* value_traits, allocator alc, enum ordmap_driver driver_type);

#endif /* _INTERFACE_FACTORY_H_ */
