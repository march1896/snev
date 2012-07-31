#ifndef _CNTR_SPECIFICATION_
#define _CNTR_SPECIFICATION_

/* 
 * This file defines the minimum interface that a container should implements
 */
typedef void (*pf_linear_cntr_init)(container cntr);

typedef void (*pf_linear_cntr_deinit)(container cntr);

typedef void (*pf_linear_cntr_clean)(container cntr);

typedef int  (*pf_linear_cntr_size)(container cntr);

typedef void* (*pf_linear_cntr_front)(container cntr);

typedef void* (*pf_linear_cntr_back)(container cntr);

typedef void (*pf_linear_cntr_add_front)(container cntr, void* obj);

typedef void (*pf_linear_cntr_add_back)(container cntr, void* obj);

typedef void* (*pf_linear_cntr_remove_front)(container cntr);

typedef void* (*pf_linear_cntr_remove_back)(container cntr);

typedef void (*pf_linear_cntr_citer_begin)(container cntr, citer* itr);

typedef void (*pf_linear_cntr_citer_end)(container cntr, citer* itr);

#endif /* _CNTR_SPECIFICATION_ */
