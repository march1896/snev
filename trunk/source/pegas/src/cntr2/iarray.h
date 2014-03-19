#ifndef _INTERFACE_ARRAY_H_
#define _INTERFACE_ARRAY_H_

#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

extern inline       void     iarray_clear           (_object* obj);
extern inline       void     iarray_foreach         (_object* obj, pf_ref_process_v process, void* context);
extern inline       int      iarray_size            (const _object* obj);
extern inline       bool     iarray_empty           (const _object* obj);
extern inline       void     iarray_add_front       (_object* obj, const unknown* __ref);
extern inline       void     iarray_add_back        (_object* obj, const unknown* __ref);
extern inline       void     iarray_remove_front    (_object* obj);
extern inline       void     iarray_remove_back     (_object* obj);
extern inline       bool     iarray_contains        (const _object* obj, const unknown* __ref);
extern inline       bool     iarray_remove          (_object* obj, const unknown* __ref);
/* the return value is a reference managed by the iarray internally, modify it on your own risk */
extern inline const unknown* iarray_front           (const _object* obj);
extern inline const unknown* iarray_back            (const _object* obj);
extern inline const unknown* iarray_at              (const _object* obj, int index);

/* return the begin iterator of the container, this iterator is constant, 
 * you could only use it for comparison, dereferencing.
 * you can not move it or set reference on it. */
extern inline const_iterator iarray_itr_begin       (const _object* obj);
extern inline const_iterator iarray_itr_end         (const _object* obj);
extern inline       iterator iarray_itr_create      (const _object* obj, itr_pos pos);
extern inline       void     iarray_itr_assign      (const _object* obj, iterator itr, itr_pos pos);
extern inline       void     iarray_itr_find        (const _object* obj, iterator itr, const unknown* __ref);
extern inline       void     iarray_itr_remove      (_object* obj, iterator itr);
/*
{
	iterator itr = iarray_itr_create(list, itr_begin);
	const_iterator end = iarray_itr_end(obj);
	for (; !itr_equals(itr, end); itr_to_next(itr)) {
		// do something on itr
	}
	itr_destroy(itr);
}
*/

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_iarray_clear)        (_object* obj);
typedef       void     (*pf_iarray_foreach)      (_object* obj, pf_ref_process_v process, void* context);
typedef       int      (*pf_iarray_size)         (const _object* obj);
typedef       bool     (*pf_iarray_empty)        (const _object* obj);
typedef const unknown* (*pf_iarray_front)        (const _object* obj);
typedef const unknown* (*pf_iarray_back)         (const _object* obj);
typedef const unknown* (*pf_iarray_at)           (const _object* obj, int index);
typedef       void     (*pf_iarray_add_front)    (_object* obj, const unknown* n_ref);
typedef       void     (*pf_iarray_add_back)     (_object* obj, const unknown* n_ref);
typedef       void     (*pf_iarray_remove_front) (_object* obj);
typedef       void     (*pf_iarray_remove_back)  (_object* obj);
typedef       bool     (*pf_iarray_contains)     (const _object* obj, const unknown* __ref);
typedef       bool     (*pf_iarray_remove)       (_object* obj, const unknown* __ref);

typedef const_iterator (*pf_iarray_itr_begin)    (const _object* obj);
typedef const_iterator (*pf_iarray_itr_end)      (const _object* obj);
typedef       iterator (*pf_iarray_itr_create)   (const _object* obj, itr_pos pos);
typedef       void     (*pf_iarray_itr_assign)   (const _object* obj, /*out*/iterator itr, itr_pos pos);
typedef       void     (*pf_iarray_itr_find)     (const _object* obj, /*out*/iterator itr, const unknown* __ref);
typedef       void     (*pf_iarray_itr_remove)   (_object* obj, iterator itr);

struct iarray_vtable {
	/* public */
	pf_iarray_clear          __clear;
	pf_iarray_foreach        __foreach;
	pf_iarray_size           __size;
	pf_iarray_empty          __empty;
	pf_iarray_front          __front;
	pf_iarray_back           __back;
	pf_iarray_at             __at;
	pf_iarray_add_front      __add_front;
	pf_iarray_add_back       __add_back;
	pf_iarray_remove_front   __remove_front;
	pf_iarray_remove_back    __remove_back;
	pf_iarray_contains       __contains;
	pf_iarray_remove         __remove;

	pf_iarray_itr_begin      __itr_begin;
	pf_iarray_itr_end        __itr_end;
	pf_iarray_itr_create     __itr_create;
	pf_iarray_itr_assign     __itr_assign;
	pf_iarray_itr_find       __itr_find;
	pf_iarray_itr_remove     __itr_remove;
};

#endif /* _INTERFACE_ARRAY_H_ */
