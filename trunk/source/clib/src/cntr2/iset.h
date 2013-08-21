#ifndef _INTERFACE_SET_H_
#define _INTERFACE_SET_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void iset_destroy         (iobject* iq);
extern inline void iset_clear           (iobject* iq);
extern inline int  iset_size            (iobject* iq);
extern inline void iset_insert          (iobject* iq, void* __ref);
extern inline bool iset_contains        (const iobject* iq, void* __ref);

extern inline iterator iset_itr_create  (iobject* iq, itr_pos pos);
extern inline void     iset_itr_assign  (iobject* iq, iterator itr, itr_pos pos);
extern inline void     iset_itr_find    (iobject* iq, iterator itr, void* __ref);
extern inline void*    iset_itr_remove  (iobject* iq, iterator itr);
/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline const iterator iset_itr_begin   (iobject* iq);
extern inline const iterator iset_itr_end     (iobject* iq);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_iset_destroy)     (object* c);
typedef void     (*pf_iset_clear)       (object* c);
typedef int      (*pf_iset_size)        (object* c);
typedef void     (*pf_iset_insert)      (object* c, void* __ref);
typedef bool     (*pf_iset_contains)    (object* c, void* __ref);

typedef       iterator (*pf_iset_itr_create) (object* c, itr_pos pos);
typedef       void     (*pf_iset_itr_assign) (object* c, iterator itr, itr_pos pos);
typedef       void     (*pf_iset_find)       (object* c, iterator itr, void* __ref);
typedef       void*    (*pf_iset_itr_remove) (object* c, iterator itr);
typedef const iterator (*pf_iset_itr_begin)  (object* c);
typedef const iterator (*pf_iset_itr_end)    (object* c);

struct iset_vtable {
	/* public */
	pf_iset_destroy     __destroy;
	pf_iset_clear       __clear;
	pf_iset_size        __size;
	pf_iset_insert      __insert;
	pf_iset_find        __find;
	pf_iset_itr_remove      __remove;
	pf_iset_itr_create  __itr_create;
	pf_iset_itr_assign  __itr_assign;
	pf_iset_itr_begin   __itr_begin;
	pf_iset_itr_end     __itr_end;
};

#endif /* _INTERFACE_SET_H_ */
