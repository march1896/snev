#ifndef _INTERFACE_STACK_H_
#define _INTERFACE_STACK_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline       void     istack_destroy     (iobject* iq);
extern inline       void     istack_clear       (iobject* iq);
extern inline       int      istack_size        (const iobject* iq);
extern inline       bool     istack_empty       (const iobject* iq);
extern inline const void*    istack_top         (const iobject* iq);
extern inline       void     istack_push        (iobject* iq, void* ref);
extern inline       void*    istack_pop         (iobject* iq);

extern inline       iterator istack_itr_create  (const iobject* iq, itr_pos pos);
extern inline       void     istack_itr_assign  (const iobject* iq, iterator itr, itr_pos pos);
extern inline const_iterator istack_itr_begin   (const iobject* iq);
extern inline const_iterator istack_itr_end     (const iobject* iq);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_istack_destroy)     (object* c);
typedef       void     (*pf_istack_clear)       (object* c);
typedef       int      (*pf_istack_size)        (const object* c);
typedef       bool     (*pf_istack_empty)       (const object* c);
typedef const void*    (*pf_istack_top)         (const object* c);
typedef       void     (*pf_istack_push)        (object* c, void* object);
typedef       void*    (*pf_istack_pop)         (object* c);

typedef       iterator (*pf_istack_itr_create)  (const object* c, itr_pos pos);
typedef       void     (*pf_istack_itr_assign)  (const object* c, iterator itr, itr_pos pos);
typedef const_iterator (*pf_istack_itr_begin)   (const object* c);
typedef const_iterator (*pf_istack_itr_end)     (const object* c);

struct istack_vtable {
	/* public */
	pf_istack_destroy     __destroy;
	pf_istack_clear       __clear;
	pf_istack_size        __size;
	pf_istack_empty       __empty;
	pf_istack_top         __top;
	pf_istack_push        __push;
	pf_istack_pop         __pop;

	pf_istack_itr_create  __itr_create;
	pf_istack_itr_assign  __itr_assign;
	pf_istack_itr_begin   __itr_begin;
	pf_istack_itr_end     __itr_end;
};

#endif /* _INTERFACE_STACK_H_ */
