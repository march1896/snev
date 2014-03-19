#ifndef _OO_MODEL_H_
#define _OO_MODEL_H_

#include <cominc.h>

typedef struct __class_t* __class;

typedef __protocols (*pf_get_protocol)(const_unknown obj, unique_id protocol_id);

/* every object belongs to some class */
struct __object_t {
	__class is_a;
};
typedef struct __object_t *object;

/* for introspective */
struct __type_info {
	unique_id           id;
	char*               name;
};

unknown dynamic_dispatch_msg(object obj, const char *msg_name, unknown param);

struct msg_entry {
	const char *name;

	unknown    function;
};

struct __protocol_sample {
	// a protocol is an object, so it has a pointer to its class
	__class             is_a;

	struct __type_info  info;

	struct msg_table {
		struct msg_entry method_0;
		struct msg_entry method_1;
	};
};


/* in order to dispatch a message at runtime, we should have a table which contains <msg_name, msg_function_entry, param_number> */

/* in .h */
struct protocol_base_object {
	__class            is_a;
	struct __type_info info;

	struct msg_table {
		struct msg_entry object_destroy;
		struct msg_entry object_clone;
		struct msg_entry object_equals;
		struct msg_entry object_compare;
	};
} base_object;

#define static_binding_dec(msg_name, msg_type) \
	extern ret_type msg_name(object obj);

#define static_binding_def(msg_name, msg_type) \
	ret_type msg_name(object target) { \
		protocol ptc = get_protocol(target, 


void   object_destroy(object obj);
object object_clone  (object obj);
bool   object_equals (object obj, object other);
int    object_compare(object obj, object other);

#define static_bindmsg_0(ptc_type, msg_name, ret_type) \
##ret_type## ##msg_name##(object obj) { \
	protocol ptc = get_protocol(obj, ptc_type); \
	return ptc->msg_tb.##msg_name##.function(obj); \
}\

#define static_bindmsg_1(ptc_type, msg_name, ret_type, param1_type, param1_name) \
ret_type msg_name(object obj, param1_type param1_name) { \
	protocol ptc = get_protocol(obj, ptc_type); \
	return ptc->msg_table.##msg_name##.function(obj, param1_name); \
}\

/* in .c */
static_dispatch(protocol_base_object, object_destroy);

struct __class_t {
	/* every class is an object, so it has a pointer to its class */
	__class             is_a;

	/* in addition, a class should carry the infomation for the type it presents */
	struct __type_info  info;

	/* a class should implements some protocol, so we could send to an object which belongs to this class some protocol msg */
	pf_get_protocol     get_protocol;
	protocol            protocols[0];
};

struct __protocol_t {
	struct __type_info  info;

	/* mothods list */
};

struct __class_t {
	__class             is_a;

	struct __type_info  info;
	pf_get_protocol     get_protocol;

	__protocol          protocols[0];
};

typedef struct __protocol_t* __protocal;

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

	/* NOTE: this function could be in the virtual function table, but that will cause duplication */
	pf_cast                   __cast;

	/* the object should know how many interfaces it has */
	struct base_interface     __iftable[1];

	/* the object running context */
	//allocator                 __allocator;

	/* object specific members, i.e. */
	/*
	int x;
	struct someobject y;
	*/
} object;

/* The above definition only defines the memory layout of object and interface, 
 * but does not define some common property of all 'objects' like 
 * destroy and clone, the below function does these. So they should typically 
 * in a interfaces vtable struct, and then in the object's vtable, like below,
	struct __base_vtable {
		pf_oo_destroy              __destroy;
		pf_oo_clone                __clone;
	};
 */
typedef void (*pf_oo_destroy)(object* obj);

/* An object should know how to clone itself, but the clone method has transitivity.
 * that is, if an object has a clone method, all objects that it inherits or contains should 
 * have clone too, but the oo_model is not completely. 
 * i.e. we can hold non-oo-object in the container, so the container does not 
 * how to clone element stored in the container. */
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

/*
 * include the raw-pointer-object common processing functions.
 */
#include <oo_ref.h>

#endif /* _OO_MODEL_H_ */
