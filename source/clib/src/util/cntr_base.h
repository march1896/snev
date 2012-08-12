#ifndef _CNTR_BASE_H_
#define _CNTR_BASE_H_

#include <citer_base.h>
/*
 * Methods declared in this file makes up a basic container.
 */

/**
 * @brief destroy the container
 *
 * @param c handle of the container
 */
void cntr_destroy(cntr c);

/**
 * @brief clear all elements of the container.
 *
 * @param c handle of the container
 */
void cntr_clear(cntr c);

/**
 * @brief get number of elements stored in the container
 *
 * @param c handle of the container
 *
 * @return size of the container
 */
int cntr_size(cntr c); 

/**
 * @brief add an element into container
 *  for list, array add an element will always succeed, but 
 *  for trees that do not store duplicated keys, when obj is 
 *  equal to some exist element in the container, the obj may 
 *  be ingored or to substitute that element(due to the tree 
 *  property), in this case, one of them will be removed, before
 *  removed, container will call pf_preremove_cb on that object.
 *  In other words, when cntr_add is called, the obj is added 
 *  into the container, but for some reason, the container may 
 *  remove some other object or even the current object.
 *
 * @param c handle of the container
 * @param obj pointer to the object
 */
void cntr_add(cntr c, void* obj);

/**
 * @brief remove a series of element from container, based on 
 *  begin and end iterators, inclusive.
 *  if pf_preremove_cb is set, it will be called before removing 
 *  a single object.
 *
 * @param c handle of the container
 * @param begin start iterator to be removed
 * @param end end iterator to be removed
 */
void cntr_remove(cntr c, citer begin, citer end);

/**
 * @brief find a specific object
 *
 * @param c handle of the container
 * @param obj object to be found
 * @param itr the citer of the object if founded, meaningless otherwise.
 *
 * @return return true if the obj is found in c.
 */
bool cntr_find(cntr c, void* obj, citer itr);

/**
 * @brief initialize itr as the begin of the container
 *
 * @param c handle of the container
 * @param itr iterator to be initialized.
 */
void cntr_citer_begin(cntr c, citer itr);

/**
 * @brief initialize itr as the end of the container, unlike stl, end 
 *  is the last valid element in the container, not just a sentinel.
 *
 * @param c handle of the container
 * @param itr iterator to be initialized.
 */
void cntr_citer_end(cntr c, citer itr);

#endif /* _CNTR_BASE_H_ */
