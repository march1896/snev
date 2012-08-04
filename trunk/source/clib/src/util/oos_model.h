#ifndef _OOS_MODEL_H_
#define _OOS_MODEL_H_
#include <cominc.h>

typedef void* unknown;

/* 
 * Because unknown is reference, so passing it as parameter may change the reference value. And in the
 * oos model, type information are all erase by casting, so const keyword does not help.
 *
 * __in, __out gives hints to user about the use of param.
 */
#if !defined(_MSC_VER)

#define __out
#define __in

#endif
/*
 * Because our philosophy is to erase type information in oos, if we want to new a frequently 
 * used object on stack, we should use the following helpers.
 */
typedef struct __struct_twopointer_long_variable_t {
	void* never_reference_me_X;
	void* never_reference_me_Y;
} __twopointer_long_variable;

typedef struct __struct_fourpointer_long_variable_t {
	void* never_reference_me_X;
	void* never_reference_me_Y;
	void* never_reference_me_Z;
	void* never_reference_me_A;
} __fourpointer_long_variable;

/*
 * allocate an unknown object of 2 pointers length on stack.
 * unknown_name is the name of the unknown object.
 * init_unknown_address is the address that unknown object to init with.
 */
__twopointer_long_variable __twopointer_long_null;
__fourpointer_long_variable __fourpointer_long_null; 

#define alloc_two_pointers_on_stack(unknown_name, init_unknown_address) \
	__twopointer_long_variable unknown_name##_never_reference_me = init_unknown_address == NULL ?  __twopointer_long_null : *((__twopointer_long_variable*)init_unknown_address);\
	unknown unknown_name = (unknown)&(unknown_name##_never_reference_me)

#define alloc_four_pointers_on_stack(unknown_name, init_unknown_address) \
	__fourpointer_long_variable unknown_name##_never_reference_me = init_unknown_address == NULL ? __fourpointer_long_null : *((__fourpointer_long_variable*)init_unknown_address);\
	unknown unknown_name = (unknown)&(unknown_name##_never_reference_me)

#endif /* _OOS_MODEL_H_ */
