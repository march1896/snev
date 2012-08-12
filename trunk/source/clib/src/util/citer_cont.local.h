#ifndef _CITER_CONT_LOCAL_H_
#define _CITER_CONT_LOCAL_H_

#include <citer_base.local.h>

typedef int (*pf_citer_cont_dis)(const citer from, const citer to);

typedef void (*pf_citer_cont_move_n)(citer itr, int n);

typedef struct __citer_cont_vtable {
	/* from citer base */
	pf_citer_attribute __attrib;

	pf_citer_get_ref __get_ref;
	pf_citer_set_ref __set_ref;
	pf_citer_to_prev __to_prev;
	pf_citer_to_next __to_next;

	/* continuous citer specific */
	pf_citer_cont_dis __distance;
	pf_citer_cont_move_n __move_n;
} citer_cont_vtable;

typedef struct __citer_cont_body {
	citer_cont_vtable*   __vt;

	void*                connection;
} citer_cont;

#endif /* _CITER_CONT_LOCAL_H_ */
