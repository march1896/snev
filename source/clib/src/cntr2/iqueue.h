#ifndef _INTERFACE_QUEUE_H_
#define _INTERFACE_QUEUE_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>

extern inline void iqueue_destroy         (iobject* iq);
extern inline void iqueue_clear           (iobject* iq);
extern inline int  iqueue_size            (iobject* iq);
extern inline void iqueue_push            (iobject* iq, void* ref);
extern inline void* iqueue_pop            (iobject* iq);

/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline object* iqueue_itr_begin    (iobject* iq);
extern inline object* iqueue_itr_end      (iobject* iq);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_iqueue_destroy)     (object* c);
typedef void     (*pf_iqueue_clear)       (object* c);
typedef int      (*pf_iqueue_size)        (object* c);
typedef void     (*pf_iqueue_push)        (object* c, void* object);
typedef void*    (*pf_iqueue_pop)         (object* c);
typedef object*  (*pf_iqueue_itr_begin)   (object* c);
typedef object*  (*pf_iqueue_itr_end)     (object* c);


/* TODO: change the number */
#define IQUEUE_ID 0X00000101
struct iqueue_vtable {
	/* public */
	pf_iqueue_destroy     __destroy;
	pf_iqueue_clear       __clear;
	pf_iqueue_size        __size;
	pf_iqueue_push        __push;
	pf_iqueue_pop         __pop;
	pf_iqueue_itr_begin   __itr_begin;
	pf_iqueue_itr_end     __itr_end;
};

#endif /* _INTERFACE_QUEUE_H_ */
