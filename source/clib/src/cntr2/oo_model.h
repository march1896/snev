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

extern inline struct base_object* __object_from_interface(const struct base_interface* inf);
extern inline bool __is_object(unknown x);
extern inline bool __is_interface(unknown x);
/* 
 * cast everything,
 * from object to interface, from interface to object, 
 * from interface to interface(not common in c++), from object to object(which will return NULL since we 
 * have only one layer of inherit, that is object inherit from interfaces.
 */
extern inline unknown __cast(unknown x, unique_id id);

/* cast a object to one of its interfaces if you know the offset of the interface in the object */
extern inline struct base_interface* __fast_cast(unknown x, int ifoffset);
#endif /* _OO_MODEL_H_ */
