#ifndef _INTERFACE_QUEUE_H_
#define _INTERFACE_QUEUE_H_

/* the interface functions which support, used by clients */
#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

extern inline       void     iqueue_clear       (_object* obj);
extern inline       void     iqueue_foreach     (_object* obj, pf_ref_process_v process, void* context);
extern inline       int      iqueue_size        (const _object* obj);
extern inline       bool     iqueue_empty       (const _object* obj);
extern inline const unknown* iqueue_front       (const _object* obj);
extern inline const unknown* iqueue_back        (const _object* obj);
extern inline       void     iqueue_push        (_object* obj, const unknown* __ref);
extern inline       void     iqueue_pop         (_object* obj);
extern inline       iterator iqueue_itr_create  (const _object* obj, itr_pos pos);
extern inline       void     iqueue_itr_assign  (const _object* obj, iterator itr, itr_pos pos);
extern inline const_iterator iqueue_itr_begin   (const _object* obj);
extern inline const_iterator iqueue_itr_end     (const _object* obj);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_iqueue_clear)       (_object* c);
typedef       void     (*pf_iqueue_foreach)     (_object* c, pf_ref_process_v process, void* context);
typedef       int      (*pf_iqueue_size)        (const _object* c);
typedef       bool     (*pf_iqueue_empty)       (const _object* c);
typedef const unknown* (*pf_iqueue_front)       (const _object* c);
typedef const unknown* (*pf_iqueue_back)        (const _object* c);
typedef       void     (*pf_iqueue_push)        (_object* c, const unknown* object);
typedef       void     (*pf_iqueue_pop)         (_object* c);
typedef       iterator (*pf_iqueue_itr_create)  (const _object* c, itr_pos pos);
typedef       void     (*pf_iqueue_itr_assign)  (const _object* c, iterator itr, itr_pos pos);
typedef const_iterator (*pf_iqueue_itr_begin)   (const _object* c);
typedef const_iterator (*pf_iqueue_itr_end)     (const _object* c);

struct iqueue_vtable {
	/* public */
	pf_iqueue_clear       __clear;
	pf_iqueue_foreach     __foreach;
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
