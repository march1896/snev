#ifndef _CITER_BASE_LOCAL_H_
#define _CITER_BASE_LOCAL_H_

#include <cattr.h>

typedef cattr (*pf_citer_attribute)(citer);

typedef void    (*pf_citer_to_next)(citer);
typedef void    (*pf_citer_to_prev)(citer);
typedef void*   (*pf_citer_get_ref)(citer);
typedef void    (*pf_citer_set_ref)(citer, void*);

typedef struct citer_base_vtable_t {
	pf_citer_attribute __attrib;

	pf_citer_get_ref __get_ref;
	pf_citer_set_ref __set_ref;
	pf_citer_to_prev __to_prev;
	pf_citer_to_next __to_next;
} citer_base_vtable;

/*
 * Instead of * typedef unknow citer * , I expose the details of iterator to let 
 * client to declare a citer on stack.
 */
typedef struct citer_base_class_t {
	/* * pointer to specific iterator operations */
	citer_base_vtable* __vt; 

	/* * pointer to the data structure that connected objects in a container */
	void*              connection;  

} citer_base;

/*
 * helper functions 
 */

extern inline bool citer_check_attr(citer c, cattr base);

#endif /* _CITER_BASE_LOCAL_H_ */
