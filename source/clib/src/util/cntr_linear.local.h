#ifndef _CNTR_LINEAR_LOCAL_H_
#define _CNTR_LINEAR_LOCAL_H_

#include <cntr_base.local.h>

typedef void* (*pf_cntr_linear_front)(cntr c);
typedef void* (*pf_cntr_linear_back)(cntr c);
typedef void (*pf_cntr_linear_add_front)(cntr c, void* obj);
typedef void (*pf_cntr_linear_add_back)(cntr c, void* obj);
typedef void* (*pf_cntr_linear_remove_front)(cntr c);
typedef void* (*pf_cntr_linear_remove_back)(cntr c);

typedef struct cntr_linear_vtable_t {
	/* from cntr */
	pf_attribute                __attrib;
	pf_cntr_base_destroy        __destroy;
	pf_cntr_base_clear          __clear;
	pf_cntr_base_size           __size;
	pf_cntr_base_add            __add;
	pf_cntr_base_remove         __remove;
	pf_cntr_base_find           __find;
	pf_cntr_base_citer_begin    __citer_begin;
	pf_cntr_base_citer_end      __citer_end;

	/* clinear specific */
	pf_cntr_linear_front        __front;
	pf_cntr_linear_back         __back;
	pf_cntr_linear_add_front    __add_front;
	pf_cntr_linear_add_back     __add_back;
	pf_cntr_linear_remove_front __remove_front;
	pf_cntr_linear_remove_back  __remove_back;
} cntr_linear_vtable;

typedef struct cntr_linear_body_t {
	cntr_linear_vtable*     __vt;
	
	/* ... */
} cntr_linear_body;

#endif /* _CNTR_LINEAR_LOCAL_H_ */
