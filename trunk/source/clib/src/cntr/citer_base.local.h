#ifndef _CITER_BASE_LOCAL_H_
#define _CITER_BASE_LOCAL_H_

#include <cattr.h>

typedef cattr   (*pf_citer_attribute)(citer);

typedef void    (*pf_citer_to_next)(citer);
typedef void    (*pf_citer_to_prev)(citer);
typedef void*   (*pf_citer_get_ref)(citer);
typedef void    (*pf_citer_set_ref)(citer, void*);

typedef struct __citer_base_interface {
	pf_citer_attribute    __attrib;

	pf_citer_get_ref      __get_ref;
	pf_citer_set_ref      __set_ref;
	pf_citer_to_prev      __to_prev;
	pf_citer_to_next      __to_next;
} citer_base_interface;

/*
 * Instead of * typedef unknow citer * , I expose the details of iterator to let 
 * client to declare a citer on stack.
 */
typedef struct __citer_base_class {
	pf_cast               __cast;

	/* pointer to specific iterator operations */
	citer_base_interface* __base_intf; 

	/* * pointer to the data structure that connected objects in a container */
	void*                 connection;  
	
	/* TODO: show we know the container */
	void*                 param_x;

	void*                 param_y;
} citer_base;

/*
 * helper functions 
 */

extern inline bool citer_check_attr(citer c, cattr base);

#endif /* _CITER_BASE_LOCAL_H_ */
