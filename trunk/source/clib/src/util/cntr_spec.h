#ifndef _CNTR_SPECIFICATION_
#define _CNTR_SPECIFICATION_

/* 
 * This file defines the minimum interface that a container should implements
 */
typedef cntr (*pf_linear_cntr_create)(cntr c);

typedef void (*pf_linear_cntr_destroy)(cntr c);

typedef void (*pf_linear_cntr_clean)(cntr c);

typedef int  (*pf_linear_cntr_size)(cntr c);

typedef void* (*pf_linear_cntr_front)(cntr c);

typedef void* (*pf_linear_cntr_back)(cntr c);

typedef void (*pf_linear_cntr_add_front)(cntr c, void* obj);

typedef void (*pf_linear_cntr_add_back)(cntr c, void* obj);

typedef void* (*pf_linear_cntr_remove_front)(cntr c);

typedef void* (*pf_linear_cntr_remove_back)(cntr c);

typedef void (*pf_linear_cntr_citer_begin)(cntr c, citer itr);

typedef void (*pf_linear_cntr_citer_end)(cntr c, citer itr);

#endif /* _CNTR_SPECIFICATION_ */
