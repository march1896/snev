#ifndef _OO_SIMPLICIFIED_MODEL_H_
#define _OO_SIMPLICIFIED_MODEL_H_

/*
 * This is a document file, no *.c file should include this file, but all demo code could be 
 * compiled and run. It demostrates a simplicified object-oriented model in C.
 */

/*
 * To simplicify the oo model, we could make the following assumptions.
 * * All inherit relationship is single inherit, plus that client always code towards interface, so we 
 *   don't need cast anymore, since when we got an interfce, it can not cast to anything, but dynamic test
 *   is needed. Considering the following class derivation system, in both JAVA and C++, we should take care
 *   about dynamic cast IA to CY, IA to IC, but in OOS model, we have a single inherited tree, and only leaves 
 *   are concrete classes, that means all kinds of cast from bottom to up will be fine, each type in the bottom
 *   will contains all information in the top, and will have and extended information. By default, dynamic cast
 *   is to test if an object can be apply to a function set,
 *                                                                                                        
 *    IA       IB      IC    *    IA       IB      IC     *               IS                 *                                               
 *      \      / \     /     *      \      / \     /      *              / \                 *                                                
 *       \    /   \   /      *       \    /   \   /       *             /   \                *                                                
 *        \  /     \ /       *        \  /     \ /        *            /     \               *                                                
 *         CX      CY        *         CX      CY         *           IA      IB             *                                               
 *        /  \     / \       *         |        |         *          / \       |             *                  
 *       /    \   /   \      *         |        |         *         /   \      |             *                  
 *      /      \ /     \     *         |        |         *        /     \     |             *                  
 *    DA       DA      DC    *         DX      DY         *      CX      CY    CZ            *                  
 *          C++              *           JAVA             *         OOS                      *                      
 *                                                                                                        
 *
 * * There is only inherit relationship between interface and class, that means the inherit depth will 
 *   at most be one. In this case, we don't need to consider __this pointer, because interface will never 
 *   have data member, so all references are class members.
 */

typedef void* unknown;

typedef unsigned int type_inf;

typedef type_inf (*pf_rtti)();
typedef struct unknown_oos_base_interface_vtable_t {
	pf_rtti                       __rtti;                   
	/*
	 * interface specific methods, all of this are function callbacks
	 */
} unknown_oos_base_interface;

typedef struct unknown_oos_extend_interface_vtable_t {
	/*
	 * keep exactly the same with base interface for callbacks in the front.
	 */
} unknown_oos_extend_interface;

typedef struct unknown_oos_class_body_t {
	/* 
	 * use the first data memeber as virtual table, so we should directly convert the class body into an 
	 * interface table.
	 */
	unknown_oos_extend_interface* __vt;
	/*
	 * class specific data members.
	 */
} unknown_oos_class;

unknown unknown_oos_factory_create(unique_id uid);
/*
 * Sample codes for simplicified object-oriented model.
 */

#define __public
#define __protected
#define __private

#define interface_declare_noparam(interface_name, return_type, function_name) \
	inline return_type interface_name##_##function_name(unknown);\
	typedef return_type (*pf_##interface_name##_##function_name)(unknown);

#define interface_declare_oneparam(interface_name, return_type, function_name, param_type) \
	inline return_type interface_name##_##function_name(unknown, param_type);\
	typedef return_type (*pf_##interface_name##_##function_name)(unknown, param_type);

#define interface_declare_twoparams(interface_name, return_type, function_name, param_type_first, param_type_second) \
	inline return_type interface_name##_##function_name(unknown, param_type_first, param_type_second);\
	typedef return_type (*pf_##interface_name##_##function_name)(unknown, param_type_first, param_type_second);

#define interface_struct_start(interface_name) \
	typedef struct interface_name##_interface_t {

#define interface_struct_end(interface_name) \
	} interface_name##_interface;

#define interface_struct_function_declare(interface_name, function_name) \
	pf_##interface_name##_##function_name __##function_name;

#define interface_implement_noparam(interface_name, return_type, function_name) \
	inline return_type interface_name##_##function_name(unknown unk) {\
		interface_name##_interface* itf = (interface_name##_interface*)(((unknown_oos_class*)unk)->__vt);\
		return itf->__##function_name(unk);\
	}

#define interface_implement_oneparam(interface_name, return_type, function_name, param_type) \
	inline return_type interface_name##_##function_name(unknown unk, param_type param) {\
		interface_name##_interface* itf = (interface_name##_interface*)(((unknown_oos_class*)unk)->__vt);\
		return itf->__##function_name(unk, param);\
	}

#define interface_implement_twoparams(interface_name, return_type, function_name, param_type_first, param_type_second) \
	inline return_type interface_name##_##function_name(unknown unk, param_type_first first, param_type_second second) {\
		interface_name##_interface* itf = (interface_name##_interface*)(((unknown_oos_class*)unk)->__vt);\
		return rtf->__##function_name(unk, first, second);\
	}

/*
 * prototype
 * int fly(int);
 * void breath()
 * void eat(int);
 */

/* public */
interface_declare_oneparam(bird, int, fly, int);
interface_declare_noparam(bird, void, breath);
interface_declare_oneparam(bird, void, eat, int);

/* protected */
interface_struct_start(bird)
interface_struct_function_declare(bird, fly)
interface_struct_function_declare(bird, breath)
interface_struct_function_declare(bird, eat)
interface_struct_end(bird)

interface_implement_oneparam(bird, int, fly, int);
interface_implement_noparam(bird, void, breath);
interface_implement_oneparam(bird, void, eat, int);
/***********************************************************************************************/

/* swallow implement */

#include <stdio.h>
#include <stdlib.h>

__protected int swallow_local_fly(unknown obj, int);
__protected void swallow_local_breath(unknown obj);
__protected void swallow_local_eat(unknown obj, int);

__private static bird_interface swallow_bird_vt = {
	swallow_local_fly,
	swallow_local_breath,
	swallow_local_eat
};

__private typedef struct swallow_body_t {
	bird_interface*  __vt;

	int              age;
	int              anything;
} swallow_class;

__protected unknown swallow_create() {
	swallow_class* swc = (swallow_class*)malloc(sizeof(swallow_class));

	swc->__vt = &swallow_bird_vt;
	swc->age = 0;
	swc->anything = 0;
	return (unknown)swc;
}

__protected void swallow_destroy(unknown obj) {
	swallow_class* swc = (swallow_class*)obj;

	free(swc);
}

__protected int swallow_local_fly(unknown obj, int nfly) {
	swallow_class* swc = (swallow_class*)obj;

	printf("swallow old fly is %d\n", swc->anything);
	swc->anything = nfly;

	return 0;
}

__protected void swallow_local_breath(unknown obj) {
	swallow_class* swc = (swallow_class*)obj;

	printf("swallow breath\n");
}

__protected void swallow_local_eat(unknown obj, int neat) {
	swallow_class* swc = (swallow_class*)obj;

	printf("swallow eat %d\n", neat);
}

int magpie_local_fly(unknown obj, int);
void magpie_local_breath(unknown obj);
void magpie_local_eat(unknown obj, int);

static bird_interface magpie_bird_vt = {
	magpie_local_fly,
	magpie_local_breath,
	magpie_local_eat
};

typedef struct magpie_body_t {
	bird_interface*  __vt;

	int              age;
	int              anything;
} magpie_class;

unknown magpie_create() {
	magpie_class* mag = (magpie_class*)malloc(sizeof(magpie_class));

	mag->__vt = &magpie_bird_vt;
	mag->age = 10;
	mag->anything = 100;

	return (unknown)mag;
}

void magpie_destroy(unknown obj) {
	magpie_class* mag = (magpie_class*)obj;

	free(mag);
}

int magpie_local_fly(unknown obj, int nfly) {
	magpie_class* swc = (magpie_class*)obj;

	printf("magpie old fly is %d\n", swc->anything);
	swc->anything = nfly;

	return 0;
}

void magpie_local_breath(unknown obj) {
	magpie_class* swc = (magpie_class*)obj;

	printf("magpie breath\n");
}

void magpie_local_eat(unknown obj, int neat) {
	magpie_class* swc = (magpie_class*)obj;

	printf("magpie eat %d\n", neat);
}

void __run_oos_test() {
	unknown swa = (unknown)swallow_create();
	unknown mag = (unknown)magpie_create();

	bird_fly(swa, 8);
	bird_fly(swa, 10);
	bird_breath(swa);
	bird_eat(swa, 100);

	bird_fly(mag, 8);
	bird_fly(mag, 10);
	bird_breath(mag);
	bird_eat(mag, 100);
	swallow_destroy(swa);
	magpie_destroy(mag);
}

#endif /* _OO_SIMPLICIFIED_MODEL_H_ */

