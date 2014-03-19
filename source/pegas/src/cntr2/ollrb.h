#ifndef _OBJECT_LEFTLEAN_REDBLACK_TREE_H_
#define _OBJECT_LEFTLEAN_REDBLACK_TREE_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

/* if alc is NULL, create will create an osplay with multi-pool allocator to gain best efficiency */
_object*  ollrb_create          (unknown_traits* content_traits, allocator alc);

void     ollrb_destroy         (_object* o);
_object*  ollrb_clone           (const _object* o);
bool     ollrb_equals          (const _object* o, const _object* other);
int      ollrb_compare_to      (const _object* o, const _object* other);
hashcode ollrb_hashcode        (const _object* o);
void     ollrb_clear           (_object* o);
int      ollrb_size            (const _object* o);
bool     ollrb_empty           (const _object* o);
void     ollrb_insert_s        (_object* o, const unknown* __ref);
void     ollrb_replace_s       (_object* o, const unknown* __ref);
void     ollrb_insert_m        (_object* o, const unknown* __ref);
bool     ollrb_contains        (const _object* o, const unknown* __ref);
int      ollrb_count           (const _object* o, const unknown* __ref);
bool     ollrb_remove_s        (_object* o, const unknown* __ref);
int      ollrb_remove_m        (_object* o, const unknown* __ref);

iterator ollrb_itr_create      (const _object* o, itr_pos pos);
void     ollrb_itr_assign      (const _object* o, iterator itr, itr_pos pos);
void     ollrb_itr_find        (const _object* o, iterator itr, const void* __ref);
void     ollrb_itr_find_lower  (const _object* o, iterator itr, const void* __ref);
void     ollrb_itr_find_upper  (const _object* o, iterator itr, const void* __ref);
void     ollrb_itr_remove      (_object* o, iterator itr);

const_iterator ollrb_itr_begin (const _object* o);
const_iterator ollrb_itr_end   (const _object* o);

#endif /* _OBJECT_LEFTLEAN_REDBLACK_TREE_H_ */
