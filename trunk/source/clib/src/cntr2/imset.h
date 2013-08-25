#ifndef _INTERFACE_MULTIPLE_SET_H_
#define _INTERFACE_MULTIPLE_SET_H_

#include <oo_model.h>
#include <idef.h>

extern inline       void     imset_destroy    (iobject* i);
extern inline       void     imset_clear      (iobject* i);
extern inline       int      imset_size       (const iobject* i);
extern inline       bool     imset_empty      (const iobject* i);
extern inline       void     imset_insert     (iobject* i, void* __ref);
extern inline       bool     imset_contains   (const iobject* i, void* __ref);
/* return number of reference of __ref */
extern inline       int      imset_count      (const iobject* i, void* __ref);
/* return false if __ref is not in the set */
extern inline       bool     imset_remove     (iobject* i, void* __ref);

extern inline       iterator imset_itr_create (const iobject* i, itr_pos pos);
extern inline       void     imset_itr_assign (const iobject* i, /* out */iterator itr, itr_pos pos);
/* points to end if __ref is not in the set, or find the first element equal to __ref in the mset */
extern inline       void     imset_itr_find_lower(const iobject* i, /* out */iterator itr, void* __ref);
/* points to end if __ref is not in the set, or find the first element greater than in the mset */
extern inline       void     imset_itr_find_upper(const iobject* i, /* out */iterator itr, void* __ref);
extern inline       void*    imset_itr_remove (iobject* i, iterator itr);
/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline const_iterator imset_itr_begin  (const iobject* i);
extern inline const_iterator imset_itr_end    (const iobject* i);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void     (*pf_imset_destroy)    (object* o);
typedef       void     (*pf_imset_clear)      (object* o);
typedef       int      (*pf_imset_size)       (const object* o);
typedef       bool     (*pf_imset_empty)      (const object* o);
typedef       void     (*pf_imset_insert)     (object* o, void* __ref);
typedef       bool     (*pf_imset_contains)   (const object* o, void* __ref);
typedef       int      (*pf_imset_count)      (const object* o, void* __ref);
typedef       bool     (*pf_imset_remove)     (object* o, void* __ref);

typedef       iterator (*pf_imset_itr_create) (const object* o, itr_pos pos);
typedef       void     (*pf_imset_itr_assign) (const object* o, iterator itr, itr_pos pos);
typedef       void     (*pf_imset_itr_find_lower)(const object* o, iterator itr, void* __ref);
typedef       void     (*pf_imset_itr_find_upper)(const object* o, iterator itr, void* __ref);
typedef       void*    (*pf_imset_itr_remove) (object* o, iterator itr);
typedef const_iterator (*pf_imset_itr_begin)  (const object* o);
typedef const_iterator (*pf_imset_itr_end)    (const object* o);

struct imset_vtable {
	/* public */
	pf_imset_destroy        __destroy;
	pf_imset_clear          __clear;
	pf_imset_size           __size;
	pf_imset_empty          __empty;
	pf_imset_insert         __insert;
	pf_imset_contains       __contains;
	pf_imset_count          __count;
	pf_imset_remove         __remove;

	pf_imset_itr_create     __itr_create;
	pf_imset_itr_assign     __itr_assign;
	pf_imset_itr_find_lower __itr_find_lower;
	pf_imset_itr_find_upper __itr_find_upper;
	pf_imset_itr_remove     __itr_remove;
	pf_imset_itr_begin      __itr_begin;
	pf_imset_itr_end        __itr_end;
};

#endif /* _INTERFACE_MULTIPLE_SET_H_ */
