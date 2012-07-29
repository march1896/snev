#ifndef _CNTR_ITERATOR_H_
#define _CNTR_ITERATOR_H_

#include <cominc.h>
/* 
 * Using of iterator.
 *
 * citer begin_itr;
 * clist list;
 * 
 * clist_iter_begin(&list, &begin_itr);
 * 
 * for (clist_init_itr(&list, &begin_itr);
 *     citer_valid(begin_itr);
 *     citer_to_next(begin_itr)) {
 *     struct object* obj = (object*)citer_obj(begin_itr);
 * }
 * 
 * citer end_itr;
 * clist_itr_end(&list, &end_itr);
 * 
 * quick_sort(&begin_itr, &end_itr);
 *
 */

/*
 * Below is interface that a container who use iterator must provide.
 */

typedef struct cntr_iterator_t citer;
typedef bool (*pf_citer_valid)(citer* itr);
typedef void (*pf_citer_to_next)(citer* itr);
typedef void (*pf_citer_to_prev)(citer* itr);
typedef void* (*pf_citer_get_ref)(citer* itr);

struct cntr_iterator_operations_t {
	pf_citer_valid   valid;
	pf_citer_get_ref get_ref;
	pf_citer_to_prev to_prev;
	pf_citer_to_next to_next;
};
typedef struct cntr_iterator_operations_t citer_operations;

struct cntr_iterator_t {
	/* pointer to the data structure that connected objects in a container */
	void* connection;  

	/* pointer to specific iterator operations */
	citer_operations* ops; 

};

inline bool citer_valid(citer* itr) {
	return (itr->ops)->valid(itr->connection);
}

inline void citer_to_next(citer* itr) {
	(itr->ops)->to_next(itr->connection);
}

inline void citer_to_prev(citer* itr) {
	(itr->ops)->to_prev(itr->connection);
}

inline void* citer_ref(citer* itr) {
	return (itr->ops)->get_ref(itr->connection);
}

#endif /* _CNTR_ITERATOR_H_ */
