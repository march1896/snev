#ifndef _INTERFACE_LIST_H_
#define _INTERFACE_LIST_H_

#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

extern inline       void     ilist_clear           (_object* obj);
extern inline       void     ilist_foreach         (_object* obj, pf_ref_process_v process, void* context);
extern inline       int      ilist_size            (const _object* obj);
extern inline       bool     ilist_empty           (const _object* obj);
/* the return value is a reference managed by the ilist internally, modify it on your own risk */
extern inline const unknown* ilist_front           (const _object* obj);
extern inline const unknown* ilist_back            (const _object* obj);
extern inline       void     ilist_add_front       (_object* obj, const unknown* __ref);
extern inline       void     ilist_add_back        (_object* obj, const unknown* __ref);
extern inline       void     ilist_remove_front    (_object* obj);
extern inline       void     ilist_remove_back     (_object* obj);
extern inline       bool     ilist_contains        (const _object* obj, const unknown* __ref);
extern inline       bool     ilist_remove          (_object* obj, const unknown* __ref);

/* return the begin iterator of the container, this iterator is constant, 
 * you could only use it for comparison, dereferencing.
 * you can not move it or set reference on it. */
extern inline const_iterator ilist_itr_begin       (const _object* obj);
extern inline const_iterator ilist_itr_end         (const _object* obj);
extern inline       iterator ilist_itr_create      (const _object* obj, itr_pos pos);
extern inline       void     ilist_itr_assign      (const _object* obj, iterator itr, itr_pos pos);
extern inline       void     ilist_itr_find        (const _object* obj, iterator itr, const unknown* __ref);
extern inline       void     ilist_itr_remove      (_object* obj, iterator itr);
extern inline       void     ilist_itr_insert_before (_object* obj, iterator itr, const unknown* __ref);
extern inline       void     ilist_itr_insert_after  (_object* obj, iterator itr, const unknown* __ref);
/*
{
	iterator itr = ilist_itr_create(list, itr_begin);
	const_iterator end = ilist_itr_end(obj);
	for (; !itr_equals(itr, end); itr_to_next(itr)) {
		// do something on itr
	}
	itr_destroy(itr);
}
*/

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_ilist_clear)        (_object* obj);
typedef       void     (*pf_ilist_foreach)      (_object* obj, pf_ref_process_v process, void* context);
typedef       int      (*pf_ilist_size)         (const _object* obj);
typedef       bool     (*pf_ilist_empty)        (const _object* obj);
typedef const unknown* (*pf_ilist_front)        (const _object* obj);
typedef const unknown* (*pf_ilist_back)         (const _object* obj);
typedef       void     (*pf_ilist_add_front)    (_object* obj, const unknown* n_ref);
typedef       void     (*pf_ilist_add_back)     (_object* obj, const unknown* n_ref);
typedef       void     (*pf_ilist_remove_front) (_object* obj);
typedef       void     (*pf_ilist_remove_back)  (_object* obj);
typedef       bool     (*pf_ilist_contains)     (const _object* obj, const unknown* __ref);
typedef       bool     (*pf_ilist_remove)       (_object* obj, const unknown* __ref);

typedef const_iterator (*pf_ilist_itr_begin)    (const _object* obj);
typedef const_iterator (*pf_ilist_itr_end)      (const _object* obj);
typedef       iterator (*pf_ilist_itr_create)   (const _object* obj, itr_pos pos);
typedef       void     (*pf_ilist_itr_assign)   (const _object* obj, /*out*/iterator itr, itr_pos pos);
typedef       void     (*pf_ilist_itr_find)     (const _object* obj, /*out*/iterator itr, const unknown* __ref);
typedef       void     (*pf_ilist_itr_remove)   (_object* obj, iterator itr);
typedef       void     (*pf_ilist_itr_insert_before)(_object* obj, iterator itr, const unknown* n_ref);
typedef       void     (*pf_ilist_itr_insert_after) (_object* obj, iterator itr, const unknown* n_ref);

struct ilist_vtable {
	/* public */
	pf_ilist_clear          __clear;
	pf_ilist_foreach        __foreach;
	pf_ilist_size           __size;
	pf_ilist_empty          __empty;
	pf_ilist_front          __front;
	pf_ilist_back           __back;
	pf_ilist_add_front      __add_front;
	pf_ilist_add_back       __add_back;
	pf_ilist_remove_front   __remove_front;
	pf_ilist_remove_back    __remove_back;
	pf_ilist_contains       __contains;
	pf_ilist_remove         __remove;

	pf_ilist_itr_begin      __itr_begin;
	pf_ilist_itr_end        __itr_end;
	pf_ilist_itr_create     __itr_create;
	pf_ilist_itr_assign     __itr_assign;
	pf_ilist_itr_find       __itr_find;
	pf_ilist_itr_remove     __itr_remove;
	pf_ilist_itr_insert_before  __itr_insert_before;
	pf_ilist_itr_insert_after   __itr_insert_after;
};

#endif /* _INTERFACE_LIST_H_ */
