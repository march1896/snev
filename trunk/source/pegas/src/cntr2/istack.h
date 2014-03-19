#ifndef _INTERFACE_STACK_H_
#define _INTERFACE_STACK_H_

/* the interface functions which support, used by clients */
#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

extern inline       void     istack_clear       (_object* obj);
extern inline       void     istack_foreach     (_object* obj, pf_ref_process_v process, void* context);
extern inline       int      istack_size        (const _object* obj);
extern inline       bool     istack_empty       (const _object* obj);
extern inline const unknown* istack_top         (const _object* obj);
extern inline       void     istack_push        (_object* obj, const unknown* __ref);
extern inline       void     istack_pop         (_object* obj);

extern inline       iterator istack_itr_create  (const _object* obj, itr_pos pos);
extern inline       void     istack_itr_assign  (const _object* obj, iterator itr, itr_pos pos);
extern inline const_iterator istack_itr_begin   (const _object* obj);
extern inline const_iterator istack_itr_end     (const _object* obj);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_istack_clear)       (_object* c);
typedef       void     (*pf_istack_foreach)     (_object* c, pf_ref_process_v process, void* context);
typedef       int      (*pf_istack_size)        (const _object* c);
typedef       bool     (*pf_istack_empty)       (const _object* c);
typedef const unknown* (*pf_istack_top)         (const _object* c);
typedef       void     (*pf_istack_push)        (_object* c, const unknown* object);
typedef       void     (*pf_istack_pop)         (_object* c);

typedef       iterator (*pf_istack_itr_create)  (const _object* c, itr_pos pos);
typedef       void     (*pf_istack_itr_assign)  (const _object* c, iterator itr, itr_pos pos);
typedef const_iterator (*pf_istack_itr_begin)   (const _object* c);
typedef const_iterator (*pf_istack_itr_end)     (const _object* c);

struct istack_vtable {
	/* public */
	pf_istack_clear       __clear;
	pf_istack_foreach     __foreach;
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
