#ifndef _CNTR_INTERATOR_PRIVATE_
#define _CNTR_INTERATOR_PRIVATE_

typedef bool (*pf_citer_valid)(citer*);
typedef void (*pf_citer_to_next)(citer*);
typedef void (*pf_citer_to_prev)(citer*);
typedef void* (*pf_citer_get_ref)(citer*);
typedef void (*pf_citer_set_ref)(citer*, void*);
typedef int (*pf_citer_cntr_size)(citer*);

typedef struct cntr_iterator_operations_t {
	pf_citer_valid   valid;
	pf_citer_get_ref get_ref;
	pf_citer_set_ref set_ref;
	pf_citer_to_prev to_prev;
	pf_citer_to_next to_next;
	pf_citer_cntr_size cntr_size;
} citer_operations;

typedef struct cntr_iterator_t {
	/* 
	 * pointer to the data structure that connected objects in a container 
	 */
	void* connection;  

	/* 
	 * pointer to the container structure.
	 */
	void* container;

	/* 
	 * pointer to specific iterator operations
	 */
	citer_operations* ops; 
} cntr_iterator;

#endif /* _CNTR_INTERATOR_PRIVATE_ */
