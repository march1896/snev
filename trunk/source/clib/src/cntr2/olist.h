#ifndef _OBJECT_DOUBLE_LINK_LIST_H_
#define _OBJECT_DOUBLE_LINK_LIST_H_

#include <oo_model.h>
#include <ifactory.h>

/* this interface inherit ilist, so it implements all method from ilist */

object*        o_dlist_create          ();
object*        o_dlist_create_v        (allocator alc, pf_dispose dispose);

void           o_dlist_destroy         (      object* o);
void           o_dlist_clear           (      object* o);
int            o_dlist_size            (const object* o);
bool           o_dlist_empty           (const object* o);
const void*    o_dlist_front           (const object* o);
const void*    o_dlist_back            (const object* o);
void           o_dlist_add_front       (      object* o, void* __ref);
void           o_dlist_add_back        (      object* o, void* __ref);
void*          o_dlist_remove_front    (      object* o);
void*          o_dlist_remove_back     (      object* o);
bool           o_dlist_contains        (const object* o, void* __ref);
bool           o_dlist_remove          (      object* o, void* __ref);
iterator       o_dlist_itr_create      (const object* o, itr_pos pos);
void           o_dlist_itr_assign      (const object* o, iterator itr, itr_pos pos);
void           o_dlist_itr_find        (const object* o, iterator itr, void* __ref);
void*          o_dlist_itr_remove      (      object* o, iterator itr);
void           o_dlist_insert_before   (      object* o, iterator itr, void* n_ref);
void           o_dlist_insert_after    (      object* o, iterator itr, void* n_ref);
const_iterator o_dlist_itr_begin       (const object* o);
const_iterator o_dlist_itr_end         (const object* o);

#endif /* _OBJECT_DOUBLE_LINK_LIST_H_ */
