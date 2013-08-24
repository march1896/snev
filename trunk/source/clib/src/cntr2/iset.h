#ifndef _INTERFACE_SET_H_
#define _INTERFACE_SET_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void     iset_destroy          (iobject* ic);
extern inline void     iset_clear            (iobject* ic);
extern inline int      iset_size             (const iobject* ic);
extern inline void     iset_insert           (iobject* ic, void* __ref);
extern inline bool     iset_contains         (const iobject* ic, void* __ref);
extern inline bool     iset_remove           (iobject* ic, void* __ref);

extern inline iterator iset_itr_create       (const iobject* ic, itr_pos pos);
extern inline void     iset_itr_assign       (const iobject* ic, /* out */iterator itr, itr_pos pos);
extern inline void     iset_itr_find         (const iobject* ic, /* out */iterator itr, void* __ref);
extern inline void*    iset_itr_remove       (iobject* ic, iterator itr);
/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline const_iterator iset_itr_begin  (const iobject* ic);
extern inline const_iterator iset_itr_end    (const iobject* ic);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_iset_destroy)    (object* c);
typedef       void     (*pf_iset_clear)      (object* c);
typedef       int      (*pf_iset_size)       (const object* c);
typedef       void     (*pf_iset_insert)     (object* c, void* __ref);
typedef       bool     (*pf_iset_contains)   (const object* c, void* __ref);
typedef       bool     (*pf_iset_remove)     (object* c, void* __ref);

typedef       iterator (*pf_iset_itr_create) (const object* c, itr_pos pos);
typedef       void     (*pf_iset_itr_assign) (const object* c, iterator itr, itr_pos pos);
typedef       void     (*pf_iset_itr_find)   (const object* c, iterator itr, void* __ref);
typedef       void*    (*pf_iset_itr_remove) (object* c, iterator itr);
typedef const_iterator (*pf_iset_itr_begin)  (const object* c);
typedef const_iterator (*pf_iset_itr_end)    (const object* c);

struct iset_vtable {
	/* public */
	pf_iset_destroy     __destroy;
	pf_iset_clear       __clear;
	pf_iset_size        __size;
	pf_iset_insert      __insert;
	pf_iset_contains    __contains;
	pf_iset_remove      __remove;

	pf_iset_itr_create  __itr_create;
	pf_iset_itr_assign  __itr_assign;
	pf_iset_itr_find    __itr_find;
	pf_iset_itr_remove  __itr_remove;
	pf_iset_itr_begin   __itr_begin;
	pf_iset_itr_end     __itr_end;
};

#endif /* _INTERFACE_SET_H_ */
