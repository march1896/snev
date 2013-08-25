#ifndef _INTERFACE_QUEUE_H_
#define _INTERFACE_QUEUE_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline       void     iqueue_destroy     (iobject* iq);
extern inline       void     iqueue_clear       (iobject* iq);
extern inline       int      iqueue_size        (const iobject* iq);
extern inline       bool     iqueue_empty       (const iobject* iq);
extern inline const void*    iqueue_front       (const iobject* iq);
extern inline const void*    iqueue_back        (const iobject* iq);
extern inline       void     iqueue_push        (iobject* iq, void* ref);
extern inline       void*    iqueue_pop         (iobject* iq);
extern inline       iterator iqueue_itr_create  (const iobject* iq, itr_pos pos);
extern inline       void     iqueue_itr_assign  (const iobject* iq, iterator itr, itr_pos pos);
extern inline const_iterator iqueue_itr_begin   (const iobject* iq);
extern inline const_iterator iqueue_itr_end     (const iobject* iq);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_iqueue_destroy)     (object* c);
typedef       void     (*pf_iqueue_clear)       (object* c);
typedef       int      (*pf_iqueue_size)        (const object* c);
typedef       bool     (*pf_iqueue_empty)       (const object* c);
typedef const void*    (*pf_iqueue_front)       (const object* c);
typedef const void*    (*pf_iqueue_back)        (const object* c);
typedef       void     (*pf_iqueue_push)        (object* c, void* object);
typedef       void*    (*pf_iqueue_pop)         (object* c);
typedef       iterator (*pf_iqueue_itr_create)  (const object* c, itr_pos pos);
typedef       void     (*pf_iqueue_itr_assign)  (const object* c, iterator itr, itr_pos pos);
typedef const_iterator (*pf_iqueue_itr_begin)   (const object* c);
typedef const_iterator (*pf_iqueue_itr_end)     (const object* c);

struct iqueue_vtable {
	/* public */
	pf_iqueue_destroy     __destroy;
	pf_iqueue_clear       __clear;
	pf_iqueue_size        __size;
	pf_iqueue_empty       __empty;
	pf_iqueue_front       __front;
	pf_iqueue_back        __back;
	pf_iqueue_push        __push;
	pf_iqueue_pop         __pop;

	pf_iqueue_itr_create  __itr_create;
	pf_iqueue_itr_assign  __itr_assign;
	pf_iqueue_itr_begin   __itr_begin;
	pf_iqueue_itr_end     __itr_end;
};

#endif /* _INTERFACE_QUEUE_H_ */
