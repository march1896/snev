#ifndef _OO_SIMPLICIFIED_H_
#define _OO_SIMPLICIFIED_H_

/*
 * This is a document file, no *.c file should include this file, but all demo code could be 
 * compiled and run. It demostrates a simplicified object-oriented model in C.
 */

#include <oo_model.h>

/*
 * To simplicify the oo model, we could make the following assumptions.
 * * All inherit relationship is single inherit, plus that client always code towards interface, so we 
 *   don't need cast anymore, since when we got a interfce, it can not cast to anything.
 * * There is only inherit relationship between interface and class, that means the inherit depth will 
 *   at most be one. In this case, we don't need to consider __this pointer, because interface will never 
 *   have data member, so all references are class members.
 */

typedef void* unknown;

typedef int unique_id;

typedef struct unknown_oos_interface_vtable_t {
	/*
	 * interface specific methods, all of this are function callbacks
	 */
} unknown_oos_interface;

typedef struct unknown_oos_class_body_t {
	/* 
	 * use the first data memeber as virtual table, so we should directly convert the class body into an 
	 * interface table.
	 */
	unknown_oos_interface* __vt;

	/*
	 * class specific data members.
	 */
} unknown_oos_class;

unknown unknown_oos_factory_create(unique_id uid);

/*
 * Sample codes for simplicified object-oriented model.
 */

#define interface_declare_noparam(interface_name, return_type, function_name) \
	extern inline return_type interface_name##_##function_name(unknown);\
	typedef return_type (*pf_##interface_name##_##function_name)(unknown);

#define interface_declare_oneparam(interface_name, return_type, function_name, param_type) \
	extern inline return_type interface_name##_##function_name(unknown, param_type);\
	typedef return_type (*pf_##interface_name##_##function_name)(unknown, param_type);

#define interface_declare_twoparams(interface_name, return_type, function_name, param_type_first, param_type_second) \
	extern inline return_type interface_name##_##function_name(unknown, param_type_first, param_type_second);\
	typedef return_type (*pf_##interface_name##_##function_name)(unknown, param_type_first, param_type_second);

#define interface_struct_start(interface_name) \
	typedef struct interface_name##_interface_t {

#define interface_struct_end(interface_name) \
	} interface_name##_interface;

#define interface_struct_function_declare(interface_name, function_name) \
	pf_##interface_name##_##function_name __##function_anme;

#define interface_implement_noparam(interface_name, return_type, function_name) \
	inline return_type interfaec_name##_##function_name(unknown unk) {\
		interface_name##_interface* itf = (*interface_name##_interface)unk;\
		return itf->__##function_name(unk);\
	}

#define interface_implement_oneparam(interface_name, return_type, function_name, param_type) \
	inline return_type interface_name##_##function_name(unknown unk, param_type param) {\
		interface_name##_interface* itf = (*interface_name##_interface)unk;\
		return itf->__##function_name(unk, param);\
	}

#define interface_implement_twoparams(interface_name, return_type, function_name, param_type_first, param_type_second) \
	inline return_type interface_name##_##function_name(unknown unk, param_type_first first, param_type_second second) {\
		interface_name##_interface* itf = (*interface_name##_interface)unk;\
		return rtf->__##function_name(unk, first, second);\
	}

/*
 * prototype
 * int fly(int);
 * void breath()
 * void eat(int);
 */

interface_declare_oneparam(bird, fly, void, int);
interface_declare_noparam(bird, breath, void);
interface_declare_oneparam(bird, eat, void, int);

interface_struct_start(bird)
interface_struct_function_declare(bird, fly)
interface_struct_function_declare(bird, breath)
interface_struct_function_declare(bird, eat)
interface_struct_end

/***********************************************************************************************
 * swallow implement */

#endif /* _OO_SIMPLICIFIED_H_ */
