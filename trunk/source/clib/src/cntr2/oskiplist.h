#ifndef _OBJECT_SKIPLIST_H_
#define _OBJECT_SKIPLIST_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

object*  oskiplist_create          (pf_ref_compare ref_comp);
/* if alc is NULL, create_v will create an osplay with multi-pool allocator to gain best efficiency */
object*  oskiplist_create_v        (pf_ref_compare ref_comp, allocator alc);
void     oskiplist_destroy         (object* o);
void     oskiplist_clear           (object* o);
void     oskiplist_clear_v         (object* o, pf_ref_dispose_v dispose, void* context);
int      oskiplist_size            (const object* o);
bool     oskiplist_empty           (const object* o);
void*    oskiplist_insert_s        (object* o, const void* __ref);
void     oskiplist_insert_m        (object* o, const void* __ref);
bool     oskiplist_contains        (const object* o, const void* __ref);
int      oskiplist_count           (const object* o, const void* __ref);
bool     oskiplist_remove          (object* o, void* __ref);

iterator oskiplist_itr_create      (const object* o, itr_pos pos);
void     oskiplist_itr_assign      (const object* o, iterator itr, itr_pos pos);
void     oskiplist_itr_find        (const object* o, iterator itr, const void* __ref);
void     oskiplist_itr_find_lower  (const object* o, iterator itr, const void* __ref);
void     oskiplist_itr_find_upper  (const object* o, iterator itr, const void* __ref);
void*    oskiplist_itr_remove      (object* o, iterator itr);

const_iterator oskiplist_itr_begin (const object* o);
const_iterator oskiplist_itr_end   (const object* o);

#endif /* _OBJECT_SKIPLIST_H_ */
