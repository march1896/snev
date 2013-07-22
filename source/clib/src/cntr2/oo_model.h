#ifndef _OO_MODEL_H_
#define _OO_MODEL_H_

#include <cominc.h>
/*
 * For more detail about the oo model, please see oo_model.doc.h
 */
typedef void*        unknown;
typedef unsigned int unique_id;
typedef void*        address;

typedef unknown (*pf_cast)(unknown obj, unique_id type_id);

struct base_interface {
	/* for interface, __offset is the offset of the interface from the object which implements it 
	 * for example, the first interface's offset is 0, the second is 1. */
	address                   __offset;

	unknown                   __vtable;
};

struct object {
	/* for object, address is the abstract address the object */
	address                   __offset;

	/* */
	pf_cast                   __cast;

	/* the object should know how many interfaces it has */
	struct base_interface     __iftable[1];

	/* object specific members below */
	/*
	int x;
	struct someobject y;
	*/
};

#define __address_of(x) x
#define __MAX_NUM_INTERFACE_PER_OBJECT 10

inline struct object* __object_from_interface(struct base_interface* inf) {
	struct base_interface* inf0 = inf - (unsigned int)inf->__offset;
	struct object* obj = container_of(inf0, struct object, __iftable[0]);

	return obj;
}

inline bool __is_object(unknown x) {
	struct object* obj = (struct object*)x;
	if (obj->__offset == __address_of(x)) {
		//dbg_assert(obj->__cast(x, OBJECT_ME) == x);
		return true;
	}

	return false;
}

inline bool __is_interface(unknown x) {
	struct base_interface* inf = (struct base_interface*)x;
	dbg_assert(inf->__offset <= __MAX_NUM_INTERFACE_PER_OBJECT);

	{
		struct object* obj = __object_from_interface(inf);
		if (__is_object(obj)) {
			return true;
		}
	}

	return false;
}

/* 
 * cast everything,
 * from object to interface, from interface to object, 
 * from interface to interface(not common in c++), from object to object(which will return NULL since we 
 * have only one layer of inherit, that is object inherit from interfaces.
 */
inline unknown __cast(unknown x, unique_id id) {
	if (__is_object(x)) {
		struct object* obj = ((struct object*)x);
		return obj->__cast(obj, id);
	}
	else if (__is_interface(x)) {
		struct object* obj = (struct object*)__object_from_interface(x);
		return obj->__cast(obj, id);
	}
	dbg_assert(false);

	return NULL;
}

/* cast a object to one of its interfaces if you know the offset of the interface in the object */
inline struct base_interface* __fast_cast(unknown x, int ifoffset) {
	struct object* obj = ((struct object*)x);

	dbg_assert(__is_object(obj));
	return (unknown)&obj->__iftable[ifoffset];
}

#endif /* _OO_MODEL_H_ */
