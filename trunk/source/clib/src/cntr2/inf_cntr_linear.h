#ifndef _INTERFACE_CNTR_LINEAR_H_
#define _INTERFACE_CNTR_LINEAR_H_

//struct base_interface; /* from oo_model.h */
#include <oo_model.h>
#include <cntr_def.h>

extern inline void* inf_cntr_linear_front       (struct base_interface* inf);

extern inline void* inf_cntr_linear_back        (struct base_interface* inf);

extern inline void  inf_cntr_linear_add_front   (struct base_interface* inf, void* object);

extern inline void  inf_cntr_linear_add_back    (struct base_interface* inf, void* object);

extern inline void* inf_cntr_linear_remove_front(struct base_interface* inf);

extern inline void* inf_cntr_linear_remove_back (struct base_interface* inf);

extern inline void* inf_cntr_linear_front_o       (struct base_interface* inf, unknown obj);
extern inline void* inf_cntr_linear_back_o        (struct base_interface* inf, unknown obj);
extern inline void  inf_cntr_linear_add_front_o   (struct base_interface* inf, unknown obj, void* object);
extern inline void  inf_cntr_linear_add_back_o    (struct base_interface* inf, unknown obj, void* object);
extern inline void* inf_cntr_linear_remove_front_o(struct base_interface* inf, unknown obj);
extern inline void* inf_cntr_linear_remove_back_o (struct base_interface* inf, unknown obj);

/* private parts start */
typedef void* (*pf_cntr_linear_front)           (cntr c);
typedef void* (*pf_cntr_linear_back)            (cntr c);
typedef void  (*pf_cntr_linear_add_front)       (cntr c, void* object);
typedef void  (*pf_cntr_linear_add_back)        (cntr c, void* object);
typedef void* (*pf_cntr_linear_remove_front)    (cntr c);
typedef void* (*pf_cntr_linear_remove_back)     (cntr c);

#define CNTR_LINEAR_INTERFACE_ID 0X00000002
#define CNTR_LINEAR_INTERFACE_OFFSET 1
struct inf_cntr_linear_vtable {
	pf_cntr_linear_front        __front;
	pf_cntr_linear_back         __back;
	pf_cntr_linear_add_front    __add_front;
	pf_cntr_linear_add_back     __add_back;
	pf_cntr_linear_remove_front __remove_front;
	pf_cntr_linear_remove_back  __remove_back;
};

#endif /* _INTERFACE_CNTR_LINEAR_H_*/
