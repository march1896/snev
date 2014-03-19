#ifndef _CLASS_VERIFY_SIZED_ARRAY_H_
#define _CLASS_VERIFY_SIZED_ARRAY_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

/* create an Object* list which holds instance of Object*s, the container will manage the life time of the Object*s. */
_object*        oarray_create          (unknown_traits* content_traits, allocator alc);

void           oarray_destroy         (      _object* cntr);
_object*        oarray_clone           (const _object* cntr);
bool           oarray_equals          (const _object* cntr, const _object* other);
int            oarray_compare_to      (const _object* cntr, const _object* other);
hashcode       oarray_hashcode        (const _object* cntr);

void           oarray_clear           (      _object* cntr);
void           oarray_foreach         (      _object* cntr, pf_ref_process_v process, void* context);
int            oarray_size            (const _object* cntr);
bool           oarray_empty           (const _object* cntr);
/* the return value is a reference which is allocated on the __global_default_heap, you should clean it up yourself */
const unknown* oarray_front           (const _object* cntr);
const unknown* oarray_back            (const _object* cntr);
const unknown* oarray_at              (const _object* cntr, int index);
void           oarray_add_front       (      _object* cntr, const unknown* __ref);
void           oarray_add_back        (      _object* cntr, const unknown* __ref);
void           oarray_remove_front    (      _object* cntr);
void           oarray_remove_back     (      _object* cntr);
bool           oarray_contains        (const _object* cntr, const unknown* __ref);
bool           oarray_remove          (      _object* cntr, const unknown* __ref);
iterator       oarray_itr_create      (const _object* cntr, itr_pos pos);
void           oarray_itr_assign      (const _object* cntr, iterator itr, itr_pos pos);
void           oarray_itr_find        (const _object* cntr, iterator itr, const unknown* __ref);
void           oarray_itr_remove      (      _object* cntr, iterator itr);
const_iterator oarray_itr_begin       (const _object* cntr);
const_iterator oarray_itr_end         (const _object* cntr);

#endif /* _CLASS_VERIFY_SIZED_ARRAY_H_ */
