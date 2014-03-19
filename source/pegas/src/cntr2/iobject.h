#ifndef _INTERFACE_BASE_OBJECT_H_
#define _INTERFACE_BASE_OBJECT_H_

#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

extern inline void     iobject_destroy        (_object* obj);
extern inline _object*  iobject_clone          (const _object* obj);
extern inline bool     iobject_equals         (const _object* obj, const _object* other);
extern inline int      iobject_compare_to     (const _object* obj, const _object* other);
extern inline hashcode iobject_hashcode       (const _object* obj);

extern inline bool     iobject_distinguishable(const _object* obj_a, const _object* obj_b);
extern inline bool     iobject_comparable     (const _object* obj_a, const _object* obj_b);
extern inline bool     iobject_hashable       (const _object* obj);

extern unknown_traits object_traits;

typedef void     (*pf_iobject_destroy)    (_object* obj);
typedef _object*  (*pf_iobject_clone)      (const _object* obj);
typedef bool     (*pf_iobject_equals)     (const _object* obj, const _object* other);
typedef int      (*pf_iobject_compare_to) (const _object* obj, const _object* other);
typedef hashcode (*pf_iobject_hashcode)   (const _object* obj);

struct iobject_vtable {
	pf_iobject_destroy      __destroy;
	pf_iobject_clone        __clone;
	pf_iobject_equals       __equals;
	pf_iobject_compare_to   __compare_to;
	pf_iobject_hashcode     __hashcode;
};

#endif /* _INTERFACE_BASE_OBJECT_H_ */
