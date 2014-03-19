#ifndef _INTERFACE_LIST_H_
#define _INTERFACE_LIST_H_

#include "oo_model.h"

extern inline       void     list_clear           (Object* target);
extern inline       void     list_foreach         (Object* target, ptc_ref_process_v process, void* context);
extern inline       int      list_size            (const Object* target);
extern inline       bool     list_empty           (const Object* target);
/* the return value is a reference managed by the list internally, modify it on your own risk */
extern inline const unknown* list_front           (const Object* target);
extern inline const unknown* list_back            (const Object* target);
extern inline       void     list_add_front       (Object* target, const unknown* __ref);
extern inline       void     list_add_back        (Object* target, const unknown* __ref);
extern inline       void     list_remove_front    (Object* target);
extern inline       void     list_remove_back     (Object* target);
extern inline       bool     list_contains        (const Object* target, const unknown* __ref);
extern inline       bool     list_remove          (Object* target, const unknown* __ref);

/* return the begin iterator of the container, this iterator is constant, 
 * you could only use it for comparison, dereferencing.
 * you can not move it or set reference on it. */
extern inline const_iterator list_itr_begin       (const Object* target);
extern inline const_iterator list_itr_end         (const Object* target);
extern inline       iterator list_itr_create      (const Object* target, itr_pos pos);
extern inline       void     list_itr_assign      (const Object* target, iterator itr, itr_pos pos);
extern inline       void     list_itr_find        (const Object* target, iterator itr, const unknown* __ref);
extern inline       void     list_itr_remove      (Object* target, iterator itr);
extern inline       void     list_itr_insert_before(Object* target, iterator itr, const unknown* __ref);
extern inline       void     list_itr_insert_after (Object* target, iterator itr, const unknown* __ref);
/*
{
	iterator itr = list_itr_create(list, itr_begin);
	const_iterator end = list_itr_end(target);
	for (; !itr_equals(itr, end); itr_to_next(itr)) {
		// do something on itr
	}
	itr_destroy(itr);
}
*/

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*ptc_list_clear)        (unknown* target);
typedef       void     (*ptc_list_foreach)      (unknown* target, ptc_ref_process_v process, void* context);
typedef       int      (*ptc_list_size)         (const unknown* target);
typedef       bool     (*ptc_list_empty)        (const unknown* target);
typedef const unknown* (*ptc_list_front)        (const unknown* target);
typedef const unknown* (*ptc_list_back)         (const unknown* target);
typedef       void     (*ptc_list_add_front)    (unknown* target, const unknown* n_ref);
typedef       void     (*ptc_list_add_back)     (unknown* target, const unknown* n_ref);
typedef       void     (*ptc_list_remove_front) (unknown* target);
typedef       void     (*ptc_list_remove_back)  (unknown* target);
typedef       bool     (*ptc_list_contains)     (const unknown* target, const unknown* __ref);
typedef       bool     (*ptc_list_remove)       (unknown* target, const unknown* __ref);

typedef       void     (*ptc_listitr_init)      (unknown *target, const unknown *list, itr_pos pos);

typedef const_iterator (*ptc_list_itr_begin)    (const unknown* target);
typedef const_iterator (*ptc_list_itr_end)      (const unknown* target);
typedef       iterator (*ptc_list_itr_create)   (const unknown* target, itr_pos pos);
typedef       void     (*ptc_list_itr_assign)   (const unknown* target, /*out*/iterator itr, itr_pos pos);
typedef       void     (*ptc_list_itr_find)     (const unknown* target, /*out*/iterator itr, const unknown* __ref);
typedef       void     (*ptc_list_itr_remove)   (unknown* target, iterator itr);
typedef       void     (*ptc_list_itr_insert_before)(unknown* target, iterator itr, const unknown* n_ref);
typedef       void     (*ptc_list_itr_insert_after) (unknown* target, iterator itr, const unknown* n_ref);

struct list_vtable {
	/* public */
	ptc_list_clear          clear;
	ptc_list_foreach        foreach;
	ptc_list_size           size;
	ptc_list_empty          empty;
	ptc_list_front          front;
	ptc_list_back           back;
	ptc_list_add_front      add_front;
	ptc_list_add_back       add_back;
	ptc_list_remove_front   remove_front;
	ptc_list_remove_back    remove_back;
	ptc_list_contains       contains;
	ptc_list_remove         remove;

	ptc_list_itr_begin      itr_begin;
	ptc_list_itr_end        itr_end;
	ptc_list_itr_create     itr_create;
	ptc_list_itr_assign     itr_assign;
	ptc_list_itr_find       itr_find;
	ptc_list_itr_remove     itr_remove;
	ptc_list_itr_insert_before  itr_insert_before;
	ptc_list_itr_insert_after   itr_insert_after;
};

#endif /* _INTERFACE_LIST_H_ */
