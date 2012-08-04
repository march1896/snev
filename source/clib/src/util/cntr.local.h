#ifndef _CNTR_LOCAL_H_
#define _CNTR_LOCAL_H_

#include <cntr_iterator.h>

typedef void (*pf_cntr_destroy)(cntr c);
typedef void (*pf_cntr_clear)(cntr c);
typedef int  (*pf_cntr_size)(cntr c);
typedef void (*pf_cntr_add)(cntr c, void* object);
typedef void (*pf_cntr_remove)(cntr c, citer begin, citer end);
typedef bool (*pf_cntr_find)(cntr c, void* object, __out citer result);
typedef void (*pf_cntr_citer_begin)(cntr c, citer itr);
typedef void (*pf_cntr_citer_end)(cntr c, citer itr);

typedef struct cntr_operations_interface_t {
	pf_cntr_destroy     __destroy;
	pf_cntr_clear       __clear;
	pf_cntr_size        __size;
	pf_cntr_add         __add;
	pf_cntr_remove      __remove;
	pf_cntr_find        __find;
	pf_cntr_citer_begin __citer_begin;
	pf_cntr_citer_end   __citer_end;
} cntr_operations;

typedef struct cntr_common_body_t {
	cntr_operations*    __vt;

	unknown             __cont;
} cntr_common_body;

/*
 * We use clinear and cset as the highest container interface, because linear and set have 
 * many significant difference, and can not be mixed together. Though containers have common
 * method such as add/find/remove, but due to significant efficiency difference, client should
 * choose the right container, creating a common container and operates on it is not encouraged, 
 * So these common cntr method is not exposed, but they may be useful to cntr common algorithm.
 */

void cntr_destroy(cntr c);
void cntr_clear(cntr c);
int  cntr_size(cntr c);
void cntr_add(cntr c, void* object);
void cntr_remove(cntr c, citer begin, citer end);
bool cntr_find(cntr c, void* object, citer __out result);
void cntr_citer_begin(cntr c, citer itr);
void cntr_citer_end(cntr c, citer itr);

#endif /* _CNTR_LOCAL_H_ */
