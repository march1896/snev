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

typedef struct base_interface {
	/* for interface, __offset is the offset of the interface from the object which implements it 
	 * for example, the first interface's offset is 0, the second is 1. */
	address                   __offset;

	unknown                   __vtable;

} iobject;

typedef struct base_object {
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
} object;

#define __address_of(x) x
#define __MAX_NUM_INTERFACE_PER_OBJECT 10

force_inline struct base_object* __object_from_interface(const struct base_interface* inf) {
	/* TODO: remove the const_cast */
	struct base_interface* inf0 = (struct base_interface*)inf - (unsigned int)inf->__offset;
	struct base_object* obj = container_of(inf0, struct base_object, __iftable[0]);

	return obj;
}

force_inline bool __is_object(unknown x) {
	struct base_object* obj = (struct base_object*)x;
	if (obj->__offset == __address_of(x)) {
		//dbg_assert(obj->__cast(x, OBJECT_ME) == x);
		return true;
	}

	return false;
}

force_inline bool __is_interface(unknown x) {
	struct base_interface* inf = (struct base_interface*)x;
	dbg_assert((int)(inf->__offset) <= __MAX_NUM_INTERFACE_PER_OBJECT);

	{
		struct base_object* obj = __object_from_interface(inf);
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
force_inline unknown __cast(unknown x, unique_id id) {
	if (__is_object(x)) {
		struct base_object* obj = ((struct base_object*)x);
		return obj->__cast(obj, id);
	}
	else if (__is_interface(x)) {
		struct base_object* obj = (struct base_object*)__object_from_interface(x);
		return obj->__cast(obj, id);
	}
	dbg_assert(false);

	return NULL;
}

/* cast a object to one of its interfaces if you know the offset of the interface in the object */
force_inline struct base_interface* __fast_cast(unknown x, int ifoffset) {
	struct base_object* obj = ((struct base_object*)x);

	dbg_assert(__is_object(obj));
	return (unknown)&obj->__iftable[ifoffset];
}

#endif /* _OO_MODEL_H_ */
