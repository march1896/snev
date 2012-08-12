#ifndef _CITER_BASE_H_
#define _CITER_BASE_H_

#include <cominc.h>
#include <oos_model.h>
#include <cattr.h>

typedef unknown citer;

typedef unknown cntr;

extern inline cattr    citer_attrib(citer itr);

extern inline void     citer_to_next(citer itr);

extern inline void     citer_to_prev(citer itr);

extern inline void*    citer_get_ref(citer itr);

extern inline void     citer_set_ref(citer itr, void* nref);

/*
 * define a citer on stack, assign the it as citer_as_init.
 */
#define citer_dos(citer_name, citer_as_init) alloc_two_pointers_on_stack(citer_name, citer_as_init)

#endif /* _CITER_BASE_H_ */
