#ifndef _CNTR_LINEAR_LOCAL_H_
#define _CNTR_LINEAR_LOCAL_H_

#include <cntr_base.local.h>

typedef void* (*pf_cntr_linear_front)       (cntr c);
typedef void* (*pf_cntr_linear_back)        (cntr c);
typedef void  (*pf_cntr_linear_add_front)   (cntr c, void* obj);
typedef void  (*pf_cntr_linear_add_back)    (cntr c, void* obj);
typedef void* (*pf_cntr_linear_remove_front)(cntr c);
typedef void* (*pf_cntr_linear_remove_back) (cntr c);

#define CNTR_LINEAR_INTERFACE 0X00000002
struct cntr_linear_interface {
	pf_cntr_linear_front        __front;
	pf_cntr_linear_back         __back;
	pf_cntr_linear_add_front    __add_front;
	pf_cntr_linear_add_back     __add_back;
	pf_cntr_linear_remove_front __remove_front;
	pf_cntr_linear_remove_back  __remove_back;
};

#endif /* _CNTR_LINEAR_LOCAL_H_ */
