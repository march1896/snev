#ifndef _CNTR_ITERATOR_H_
#define _CNTR_ITERATOR_H_

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
 * Design: minimum interface for iterator.
 *
 * Iterator design pattern decouples algorithm from container. So we should 
 * provide minimum interface that algorithms needed to use.
 * From C++/Java experience, iterators should provide move to prev/next, set/get
 * data reference, check if iterator is out of range(out of container).
 * In C++, the above functions are provided by operator override, by overriding
 * operator* const, operator*, operator++, operator--, operator ==.
 * In Java, four basic functions are provided, has_prev/next(), prev/next(), 
 * has_prev/next provides validation check, prev/next provides moving iterator as
 * well as dereference.
 *
 * So here we provide six basic raw operations for iterator.
 * bool valid                    (check if in container)
 * void to_next                  (move to next)
 * void to_prev                  (move to prev)
 * void set_ref                  (change the reference)
 * void* get_ref                 (get the reference)
 * bool equal					 (check if two iterators are equal)
 *
 * For some algorithm specific algorithm, we may provide more operations.
 * e.g. 
 * * could we compare the relative order of two iterator in container?
 * * could we get the size of the container for better performance?
 * TODO: provide minimum callbacks that is needed for main container algorithms.
 */

#include <cominc.h>
#include <oos_model.h>

typedef struct cntr_iterator_t citer;
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
/*
 * Instead of * typedef unknow citer * , I expose the details of iterator to let 
 * client to declare a citer on stack.
 */
struct cntr_iterator_t {
	/* 
	 * pointer to specific iterator operations
	 */
	citer_operations* ops; 

	/* 
	 * pointer to the data structure that connected objects in a container 
	 */
	void* connection;  

	void* container;
} ;




extern inline bool citer_valid(citer* itr);

extern inline bool citer_equal(citer* lhs, citer* rhs);

extern inline void citer_to_next(citer* itr);

extern inline void citer_to_prev(citer* itr);

extern inline void* citer_get_ref(citer* itr);

extern inline void citer_set_ref(citer* itr, void* nref);

extern inline int citer_cntr_size(citer* itr);

#endif /* _CNTR_ITERATOR_H_ */
