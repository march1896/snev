#ifndef _INTERFACE_SET_H_
#define _INTERFACE_SET_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void iset_destroy         (iobject* iq);
extern inline void iset_clear           (iobject* iq);
extern inline int  iset_size            (iobject* iq);
extern inline void iset_insert          (iobject* iq, void* ref);
/* TODO: make some documentation on this topic */
/* currently the find will return the end iterator of the container instead of NULL */
extern inline object* iset_find         (iobject* iq, void* ref);
extern inline void iset_remove          (iobject* iq, iobject* itr);

/* return a iterator, maybe foward/bidirectional/randomaccessed. */
extern inline object* iset_itr_begin    (iobject* iq);
extern inline object* iset_itr_end      (iobject* iq);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_iset_destroy)     (object* c);
typedef void     (*pf_iset_clear)       (object* c);
typedef int      (*pf_iset_size)        (object* c);
typedef void     (*pf_iset_insert)      (object* c, void* __ref);
typedef object*  (*pf_iset_find)        (object* c, void* __ref);
typedef void*    (*pf_iset_remove)      (object* c, iobject* itr);
typedef object*  (*pf_iset_itr_begin)   (object* c);
typedef object*  (*pf_iset_itr_end)     (object* c);

struct iset_vtable {
	/* public */
	pf_iset_destroy     __destroy;
	pf_iset_clear       __clear;
	pf_iset_size        __size;
	pf_iset_insert      __insert;
	pf_iset_find        __find;
	pf_iset_remove      __remove;
	pf_iset_itr_begin   __itr_begin;
	pf_iset_itr_end     __itr_end;
};

#endif /* _INTERFACE_SET_H_ */
