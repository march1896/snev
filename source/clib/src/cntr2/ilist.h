#ifndef _INTERFACE_LIST_H_
#define _INTERFACE_LIST_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void           ilist_destroy         (iobject* ilist);
extern inline void           ilist_clear           (iobject* ilist);
extern inline int            ilist_size            (iobject* ilist);
extern inline void           ilist_add_front       (iobject* ilist, void* n_ref);
extern inline void           ilist_add_back        (iobject* ilist, void* n_ref);
extern inline void*          ilist_remove_front    (iobject* ilist);
extern inline void*          ilist_remove_back     (iobject* ilist);
extern inline bool           ilist_contains        (iobject* ilist, void* __ref);

/* return the begin iterator of the container, this iterator is constant, 
 * you could only use it for comparison, dereferencing.
 * you can not move it or set reference on it. */
extern inline const iterator ilist_itr_begin     (iobject* ilist);
extern inline const iterator ilist_itr_end       (iobject* ilist);
extern inline       iterator ilist_itr_create    (iobject* ilist, itr_pos pos);
extern inline       void     ilist_itr_assign    (iobject* ilist, iterator itr, itr_pos pos);
extern inline       void     ilist_itr_find      (iobject* ilist, iterator itr, void* __ref);
extern inline       void*    ilist_itr_remove    (iobject* ilist, iterator itr);
extern inline       void     ilist_insert_before (iobject* ilist, iterator itr, void* n_ref);
extern inline       void     ilist_insert_after  (iobject* ilist, iterator itr, void* n_ref);

/*
{
	iterator itr = ilist_itr_create(list, itr_begin);
	const iterator end = ilist_itr_end(ilist);
	for (; !itr_equals(itr, end); itr_to_next(itr)) {
		// do something on itr
	}
	itr_destroy(itr);
}
*/

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_ilist_destroy)      (object* olist);
typedef       void     (*pf_ilist_clear)        (object* olist);
typedef       int      (*pf_ilist_size)         (object* olist);
typedef       void     (*pf_ilist_add_front)    (object* olist, void* n_ref);
typedef       void     (*pf_ilist_add_back)     (object* olist, void* n_ref);
typedef       void*    (*pf_ilist_remove_front) (object* olist);
typedef       void*    (*pf_ilist_remove_back)  (object* olist);
typedef       bool     (*pf_ilist_contains)     (object* olist, void* __ref);

typedef const iterator (*pf_ilist_itr_begin)    (object* olist);
typedef const iterator (*pf_ilist_itr_end)      (object* olist);

typedef       iterator (*pf_ilist_itr_create)   (object* ilist, itr_pos pos);
typedef       void     (*pf_ilist_itr_assign)   (object* ilist, /*out*/iterator itr, itr_pos pos);
typedef       void     (*pf_ilist_itr_find)     (object* olist, /*out*/iterator itr, void* __ref);

typedef       void*    (*pf_ilist_itr_remove)   (object* olist, iterator itr);
typedef       void     (*pf_ilist_insert_before)(object* olist, iterator itr, void* n_ref);
typedef       void     (*pf_ilist_insert_after) (object* olist, iterator itr, void* n_ref);


struct ilist_vtable {
	/* public */
	pf_ilist_destroy        __destroy;
	pf_ilist_clear          __clear;
	pf_ilist_size           __size;
	pf_ilist_add_front      __add_front;
	pf_ilist_add_back       __add_back;
	pf_ilist_remove_front   __remove_front;
	pf_ilist_remove_back    __remove_back;
	pf_ilist_contains       __contains;

	pf_ilist_itr_begin      __itr_begin;
	pf_ilist_itr_end        __itr_end;
	pf_ilist_itr_create     __itr_create;
	pf_ilist_itr_assign     __itr_assign;
	pf_ilist_itr_find       __itr_find;

	pf_ilist_itr_remove         __remove;
	pf_ilist_insert_before  __insert_before;
	pf_ilist_insert_after   __insert_after;
};

#endif /* _INTERFACE_LIST_H_ */
