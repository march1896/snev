#ifndef _CNTR_INTERATOR_PRIVATE_
#define _CNTR_INTERATOR_PRIVATE_

typedef bool    (*pf_citer_valid)(citer);
typedef void    (*pf_citer_to_next)(citer);
typedef void    (*pf_citer_to_prev)(citer);
typedef void*   (*pf_citer_get_ref)(citer);
typedef void    (*pf_citer_set_ref)(citer, void*);

typedef struct cntr_iterator_interface_t {
	pf_citer_valid   __cvalid;
	pf_citer_get_ref __get_ref;
	pf_citer_set_ref __set_ref;
	pf_citer_to_prev __to_prev;
	pf_citer_to_next __to_next;
} citer_interface;

/*
 * Instead of * typedef unknow citer * , I expose the details of iterator to let 
 * client to declare a citer on stack.
 */
typedef struct cntr_iterator_class_t {
	/* * pointer to specific iterator operations */
	citer_interface* __vt; 

	/* * pointer to the data structure that connected objects in a container */
	void*            connection;  

} cntr_iterator;

#endif /* _CNTR_INTERATOR_PRIVATE_ */
