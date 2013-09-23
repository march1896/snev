#ifndef _OO_MODEL_H_
#define _OO_MODEL_H_

#include <cominc.h>
/*
 * For more detail about the oo model, please see oo_model.doc.h
 */
typedef void*        unknown;
typedef const void*  const_unknown;
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

	/* TODO? this function could be in the virtual function table */
	pf_cast                   __cast;

	/* the object should know how many interfaces it has */
	struct base_interface     __iftable[1];

	/* object specific members below */
	/*
	int x;
	struct someobject y;
	*/
} object;

/* The above definition only defines the memory layout of object and interface, 
 * but does not define some common property of all 'objects' like 
 * destroy, copy and clone, the below function does these. So they should typically 
 * in a interfaces vtable struct, and then in the object's vtable, like below,
	struct __base_vtable {
		pf_oo_destroy              __destroy;
		pf_oo_copy                 __copy;
		pf_oo_clone                __clone;
	};
 */
typedef void (*pf_oo_destroy)(object* obj);
typedef void (*pf_oo_copy)   (const object* from, object* to);

/* An object should know how to clone itself, but the clone method has transitivity.
 * that is, if an object has a clone method, all objects that it inherit or hold should 
 * have clone too, but the oo_model is not completely, i.e. we can hold non-oo-object
 * in the container, so the container does not how to clone element stored in the container. */
typedef object* (*pf_oo_clone)(const object* obj);


#define __MAX_NUM_INTERFACE_PER_OBJECT 10
/* 
 * get the object from a given interface
 */
extern inline object* __object_from_interface(const iobject* inf);

/*
 * test if an unknown address is an object.
 * currently, this test is a little weak, since the only rule we base is that:
 * for any object, the first (void*) bytes saves the address itself.
 */
extern inline bool __is_object(const_unknown x);

/*
 * test if an unknown address is an interface.
 * if unknown x is an interface, we could found the base object's address,
 * if the deduced 'object' is a real object, then this is a interface.
 */
extern inline bool __is_interface(const_unknown x);

/* 
 * cast everything,
 * from object to interface, from interface to object, 
 * from interface to interface(not common in c++), from object to object(which will return NULL since we 
 * have only one layer of inherit, that is object inherit from interfaces.
 */
extern inline unknown __cast(const_unknown x, unique_id id);

/* 
 * cast a object to one of its interfaces if you know the offset of the interface in the object.
 */
extern inline iobject* __fast_cast(const object* x, int ifoffset);

#endif /* _OO_MODEL_H_ */
