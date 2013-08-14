#ifndef _INTERFACE_LIST_H_
#define _INTERFACE_LIST_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void ilist_destroy         (iobject* ilist);
extern inline void ilist_clear           (iobject* ilist);
extern inline int  ilist_size            (iobject* ilist);
extern inline void ilist_add_front       (iobject* ilist, void* n_ref);
extern inline void ilist_add_back        (iobject* ilist, void* n_ref);
extern inline void* ilist_remove_front   (iobject* ilist);
extern inline void* ilist_remove_back    (iobject* ilist);
extern inline void* ilist_remove         (iobject* ilist, iobject* itr);
extern inline void ilist_insert_before   (iobject* ilist, iobject* itr, void* n_ref);
extern inline void ilist_insert_after    (iobject* ilist, iobject* itr, void* n_ref);

/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline iterator ilist_itr_begin   (iobject* ilist);
extern inline iterator ilist_itr_end     (iobject* ilist);

/* 
 * TODO: if the target is not in the list, should we return NULL or
 * the sentinel end of the container? 
 * in the algorithm find(iterator begin, iterator end, void* ref)
 * it makes more sense to return an iterator equals end.
 * but here if we return the end, in the following program, we may 
 * further more 
 * 1,acquire the end, 
 * 2,do the comparison, 
 * 3,then delete two iterators.
 * instead, if we return NULL, we should always write like,
 *   itr = as_ifitr(ilist_itr_find(list, obj));
 *   ...
 *   if (itr != NULL) ifitr_destroy(itr); // which is error prone.
 */
extern inline iterator ilist_itr_find     (iobject* ilist, void* __ref);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_ilist_destroy)      (object* olist);
typedef void     (*pf_ilist_clear)        (object* olist);
typedef int      (*pf_ilist_size)         (object* olist);
typedef void     (*pf_ilist_add_front)    (object* olist, void* n_ref);
typedef void     (*pf_ilist_add_back)     (object* olist, void* n_ref);
typedef void*    (*pf_ilist_remove_front) (object* olist);
typedef void*    (*pf_ilist_remove_back)  (object* olist);

typedef iterator (*pf_ilist_itr_begin)    (object* olist);
typedef iterator (*pf_ilist_itr_end)      (object* olist);
typedef iterator (*pf_ilist_itr_find)     (object* olist, void* __ref);
typedef void*    (*pf_ilist_remove)       (object* olist, iobject* itr);
typedef void     (*pf_ilist_insert_before)(object* olist, iobject* itr, void* n_ref);
typedef void     (*pf_ilist_insert_after) (object* olist, iobject* itr, void* n_ref);

struct ilist_vtable {
	/* public */
	pf_ilist_destroy        __destroy;
	pf_ilist_clear          __clear;
	pf_ilist_size           __size;
	pf_ilist_add_front      __add_front;
	pf_ilist_add_back       __add_back;
	pf_ilist_remove_front   __remove_front;
	pf_ilist_remove_back    __remove_back;
	pf_ilist_itr_begin      __itr_begin;
	pf_ilist_itr_end        __itr_end;
	pf_ilist_itr_find       __itr_find;
	pf_ilist_remove         __remove;
	pf_ilist_insert_before  __insert_before;
	pf_ilist_insert_after   __insert_after;
};

#endif /* _INTERFACE_LIST_H_ */
