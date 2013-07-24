#ifndef _INTERFACE_LIST_H_
#define _INTERFACE_LIST_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void ilist_destroy         (iobject* iq);
extern inline void ilist_clear           (iobject* iq);
extern inline int  ilist_size            (iobject* iq);
extern inline void ilist_add_front       (iobject* iq, void* n_ref);
extern inline void ilist_add_back        (iobject* iq, void* n_ref);
extern inline void* ilist_remove_front   (iobject* iq);
extern inline void* ilist_remove_back    (iobject* iq);
extern inline void* ilist_remove         (iobject* iq, iobject* itr);
extern inline void ilist_insert_before   (iobject* iq, iobject* itr, void* n_ref);
extern inline void ilist_insert_after    (iobject* iq, iobject* itr, void* n_ref);

/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline object* ilist_itr_begin    (iobject* iq);
extern inline object* ilist_itr_end      (iobject* iq);
extern inline object* ilist_itr_find     (iobject* iq, void* __ref);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_ilist_destroy)      (object* iq);
typedef void     (*pf_ilist_clear)        (object* iq);
typedef int      (*pf_ilist_size)         (object* iq);
typedef void     (*pf_ilist_add_front)    (object* iq, void* n_ref);
typedef void     (*pf_ilist_add_back)     (object* iq, void* n_ref);
typedef void*    (*pf_ilist_remove_front) (object* iq);
typedef void*    (*pf_ilist_remove_back)  (object* iq);

typedef object*  (*pf_ilist_itr_begin)    (object* c);
typedef object*  (*pf_ilist_itr_end)      (object* c);
typedef object*  (*pf_ilist_itr_find)     (object* iq, void* __ref);
typedef void*    (*pf_ilist_remove)       (object* iq, iobject* itr);
typedef void     (*pf_ilist_insert_before)(object* iq, iobject* itr, void* n_ref);
typedef void     (*pf_ilist_insert_after) (object* iq, iobject* itr, void* n_ref);

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
