#ifndef _CNTR_SPECIFICATION_
#define _CNTR_SPECIFICATION_

/* 
 * This file defines the minimum interface that a container should implements
 */

typedef void* container;

typedef void* containee;

void (*pf_linear_cntr_init)(container cntr);

void (*pf_linear_cntr_deinit)(container cntr);

void (*pf_linear_cntr_clean)(container cntr);

int  (*pf_linear_cntr_size)(container cntr);

containee (*pf_linear_cntr_front)(container cntr);

containee (*pf_linear_cntr_back)(container cntr);

void (*pf_linear_cntr_add_front)(container cntr, containee obj);

void (*pf_linear_cntr_add_back)(container cntr, containee obj);

containee (*pf_linear_cntr_remove_front)(container cntr);

containee (*pf_linear_cntr_remove_back)(container cntr);

void (*pf_linear_cntr_citer_begin)(container cntr, citer* itr);

void (*pf_linear_cntr_citer_end)(container cntr, citer* itr);

#endif /* _CNTR_SPECIFICATION_ */
