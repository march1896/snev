#ifndef _OBJECT_SPLAYTREE_H_
#define _OBJECT_SPLAYTREE_H_

#include <cntr2/oo_model.h>
#include <cntr2/oallocator.h>

/* if alc is NULL, create_v will create an osplay with multi-pool allocator to gain best efficiency */
_object*         splayset_create          (unknown_traits* content_traits, allocator alc);
void            splayset_destroy         (_object* o);
_object*         splayset_clone           (const _object* o);
bool            splayset_equals          (const _object* o, const _object* other);
int             splayset_compare_to      (const _object* o, const _object* other);
hashcode        splayset_hashcode        (const _object* o);
void            splayset_clear           (_object* o);
int             splayset_size            (const _object* o);
bool            splayset_empty           (const _object* o);
void            splayset_insert_s        (_object* o, const unknown* __ref);
void            splayset_replace_s       (_object* o, const unknown* __ref);
void            splayset_insert_m        (_object* o, const unknown* __ref);
bool            splayset_contains        (const _object* o, const unknown* __ref);
int             splayset_count_m         (const _object* o, const unknown* __ref);
bool            splayset_remove_s        (_object* o, const unknown* __ref);
int             splayset_remove_m        (_object* o, const unknown* __ref);

iterator        splayset_itr_create      (const _object* o, itr_pos pos);
void            splayset_itr_assign      (const _object* o, iterator itr, itr_pos pos);
void            splayset_itr_find_s      (const _object* o, iterator itr, const unknown* __ref);
void            splayset_itr_find_lower_m(const _object* o, iterator itr, const unknown* __ref);
void            splayset_itr_find_upper_m(const _object* o, iterator itr, const unknown* __ref);
void            splayset_itr_remove      (_object* o, iterator itr);

const_iterator  splayset_itr_begin       (const _object* o);
const_iterator  splayset_itr_end         (const _object* o);

// TODO: should we have the detailed implementation for each container just for a virtual function call 
// performance?

// /* if alc is NULL, create_v will create an osplay with multi-pool allocator to gain best efficiency */
// object  splaymap_create_v        (pf_ref_compare ref_comp, allocator alc);
// object  splaymap_create          (pf_ref_compare ref_comp);
// void     splaymap_destroy         (object o);
// void     splaymap_clear           (object o);
// int      splaymap_size            (const Object* o);
// bool     splaymap_empty           (const Object* o);
// void*    splaymap_insert_s        (object o, const void* key, void* value);
// void     splaymap_insert_m        (object o, const void* key, void* value);
// bool     splaymap_contains        (const Object* o, const void* key);
// void*    splaymap_findbykey_s     (const Object* o, const void* key);
// int      splaymap_count_m         (const Object* o, const void* key);
// void*    splaymap_remove_s        (object o, void* key);
// void     splaymap_remove_m        (object o, void* key);
// 
// iterator splaymap_itr_create      (const Object* o, itr_pos pos);
// void     splaymap_itr_assign      (const Object* o, iterator itr, itr_pos pos);
// void     splaymap_itr_find_s      (const Object* o, iterator itr, const void* key);
// void     splaymap_itr_find_lower_m(const Object* o, iterator itr, const void* key);
// void     splaymap_itr_find_upper_m(const Object* o, iterator itr, const void* key);
// void*    splaymap_itr_remove      (object o, iterator itr);
// 
// const_iterator splaymap_itr_begin (const Object* o);
// const_iterator splaymap_itr_end   (const Object* o);


#endif /* _OBJECT_SPLAYTREE_H_ */
