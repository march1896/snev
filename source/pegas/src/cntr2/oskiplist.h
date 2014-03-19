#ifndef _OBJECT_SKIPLIST_H_
#define _OBJECT_SKIPLIST_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

/* if alc is NULL, create_v will create an osplay with multi-pool allocator to gain best efficiency */
_object*  oslist_create          (unknown_traits* content_traits, allocator alc);

void     oslist_destroy         (_object* o);
_object*  oslist_clone           (const _object* o);
bool     oslist_equals          (const _object* o, const _object* other);
int      oslist_compare_to      (const _object* o, const _object* other);
hashcode oslist_hashcode        (const _object* o);
void     oslist_clear           (_object* o);
int      oslist_size            (const _object* o);
bool     oslist_empty           (const _object* o);
void     oslist_insert_s        (_object* o, const unknown* __ref);
void     oslist_replace_s       (_object* o, const unknown* __ref);
void     oslist_insert_m        (_object* o, const unknown* __ref);
bool     oslist_contains        (const _object* o, const unknown* __ref);
int      oslist_count           (const _object* o, const unknown* __ref);
bool     oslist_remove_s        (_object* o, const unknown* __ref);
int      oslist_remove_m        (_object* o, const unknown* __ref);

iterator oslist_itr_create      (const _object* o, itr_pos pos);
void     oslist_itr_assign      (const _object* o, iterator itr, itr_pos pos);
void     oslist_itr_find        (const _object* o, iterator itr, const unknown* __ref);
void     oslist_itr_find_lower  (const _object* o, iterator itr, const unknown* __ref);
void     oslist_itr_find_upper  (const _object* o, iterator itr, const unknown* __ref);
void     oslist_itr_remove      (_object* o, iterator itr);

const_iterator oslist_itr_begin (const _object* o);
const_iterator oslist_itr_end   (const _object* o);

#endif /* _OBJECT_SKIPLIST_H_ */
