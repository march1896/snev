#ifndef _OBJECT_LEFTLEAN_REDBLACK_TREE_H_
#define _OBJECT_LEFTLEAN_REDBLACK_TREE_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

object*  ollrb_create          (pf_ref_compare ref_comp);
/* if alc is NULL, create_v will create an osplay with multi-pool allocator to gain best efficiency */
object*  ollrb_create_v        (pf_ref_compare ref_comp, allocator alc);
void     ollrb_destroy         (object* o);
void     ollrb_clear           (object* o);
void     ollrb_clear_v         (object* o, pf_ref_dispose_v dispose, void* context);
int      ollrb_size            (const object* o);
bool     ollrb_empty           (const object* o);
void*    ollrb_insert_s        (object* o, const void* __ref);
void     ollrb_insert_m        (object* o, const void* __ref);
bool     ollrb_contains        (const object* o, const void* __ref);
int      ollrb_count           (const object* o, const void* __ref);
bool     ollrb_remove          (object* o, void* __ref);

iterator ollrb_itr_create      (const object* o, itr_pos pos);
void     ollrb_itr_assign      (const object* o, iterator itr, itr_pos pos);
void     ollrb_itr_find        (const object* o, iterator itr, const void* __ref);
void     ollrb_itr_find_lower  (const object* o, iterator itr, const void* __ref);
void     ollrb_itr_find_upper  (const object* o, iterator itr, const void* __ref);
void*    ollrb_itr_remove      (object* o, iterator itr);

const_iterator ollrb_itr_begin (const object* o);
const_iterator ollrb_itr_end   (const object* o);

#endif /* _OBJECT_LEFTLEAN_REDBLACK_TREE_H_ */
