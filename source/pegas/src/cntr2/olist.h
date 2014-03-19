#ifndef _OBJECT_DOUBLE_LINK_LIST_H_
#define _OBJECT_DOUBLE_LINK_LIST_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

/* create an object list which holds instance of objects, the container will manage the life time of the objects. */
_object*        olist_create          (unknown_traits* content_traits, allocator alc);

void           olist_destroy         (      _object* o);
_object*        olist_clone           (const _object* o);
bool           olist_equals          (const _object* o, const _object* other);
int            olist_compare_to      (const _object* o, const _object* other);
hashcode       olist_hashcode        (const _object* o);

void           olist_clear           (      _object* o);
void           olist_foreach         (      _object* o, pf_ref_process_v process, void* context);
int            olist_size            (const _object* o);
bool           olist_empty           (const _object* o);
/* the return value is a reference which is allocated on the __global_default_heap, you should clean it up yourself */
const unknown* olist_front           (const _object* o);
const unknown* olist_back            (const _object* o);
void           olist_add_front       (      _object* o, const unknown* __ref);
void           olist_add_back        (      _object* o, const unknown* __ref);
void           olist_remove_front    (      _object* o);
void           olist_remove_back     (      _object* o);
bool           olist_contains        (const _object* o, const unknown* __ref);
bool           olist_remove          (      _object* o, const unknown* __ref);
iterator       olist_itr_create      (const _object* o, itr_pos pos);
void           olist_itr_assign      (const _object* o, iterator itr, itr_pos pos);
void           olist_itr_find        (const _object* o, iterator itr, const unknown* __ref);
void           olist_itr_remove      (      _object* o, iterator itr);
void           olist_insert_before   (      _object* o, iterator itr, const unknown* n_ref);
void           olist_insert_after    (      _object* o, iterator itr, const unknown* n_ref);
const_iterator olist_itr_begin       (const _object* o);
const_iterator olist_itr_end         (const _object* o);

#endif /* _OBJECT_DOUBLE_LINK_LIST_H_ */
