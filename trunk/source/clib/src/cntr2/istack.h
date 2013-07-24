#ifndef _INTERFACE_STACK_H_
#define _INTERFACE_STACK_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void istack_destroy         (iobject* iq);
extern inline void istack_clear           (iobject* iq);
extern inline int  istack_size            (iobject* iq);
extern inline void istack_push            (iobject* iq, void* ref);
extern inline void* istack_pop            (iobject* iq);

/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline object* istack_itr_begin    (iobject* iq);
extern inline object* istack_itr_end      (iobject* iq);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_istack_destroy)     (object* c);
typedef void     (*pf_istack_clear)       (object* c);
typedef int      (*pf_istack_size)        (object* c);
typedef void     (*pf_istack_push)        (object* c, void* object);
typedef void*    (*pf_istack_pop)         (object* c);
typedef object*  (*pf_istack_itr_begin)   (object* c);
typedef object*  (*pf_istack_itr_end)     (object* c);

struct istack_vtable {
	/* public */
	pf_istack_destroy     __destroy;
	pf_istack_clear       __clear;
	pf_istack_size        __size;
	pf_istack_push        __push;
	pf_istack_pop         __pop;
	pf_istack_itr_begin   __itr_begin;
	pf_istack_itr_end     __itr_end;
};

#endif /* _INTERFACE_STACK_H_ */
