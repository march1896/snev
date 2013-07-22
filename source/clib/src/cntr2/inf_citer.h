#ifndef _CITER_BASE_LOCAL_H_
#define _CITER_BASE_LOCAL_H_

#include <cntr_def.h>

extern inline void     inf_citer_base_to_next(struct base_interface* inf);
extern inline void     inf_citer_base_to_prev(struct base_interface* inf);
extern inline void*    inf_citer_base_get_ref(struct base_interface* inf);
extern inline void     inf_citer_base_set_ref(struct base_interface* inf, void* nref);

extern inline void     inf_citer_base_to_next_o(struct base_interface* inf, unknown obj);
extern inline void     inf_citer_base_to_prev_o(struct base_interface* inf, unknown obj);
extern inline void*    inf_citer_base_get_ref_o(struct base_interface* inf, unknown obj);
extern inline void     inf_citer_base_set_ref_o(struct base_interface* inf, unknown obj, void* nref);

typedef       void     (*pf_citer_to_next)(citer);
typedef       void     (*pf_citer_to_prev)(citer);
typedef       void*    (*pf_citer_get_ref)(citer);
typedef       void     (*pf_citer_set_ref)(citer, void*);

/* TODO: change the number */
#define CITER_BASE_INTERFACE_ID 0X00000005
#define CITER_BASE_INTERFACE_OFFSET 0
struct inf_citer_base_vtable {
	pf_citer_get_ref    __get_ref;
	pf_citer_set_ref    __set_ref;
	pf_citer_to_prev    __to_prev;
	pf_citer_to_next    __to_next;
};


/*
 * Return distance between from and to, inclusive, that is if from == to, 
 * distance between them is one.
 */
extern inline int      inf_citer_cont_dis   (const struct base_interface* from, const struct base_interface* to);
extern inline void     inf_citer_cont_move_n(struct base_interface* inf, int dis);

extern inline int      inf_citer_cont_dis_o   (const struct base_interface* , unknown objfrom, const struct base_interface* to);
extern inline void     inf_citer_cont_move_n_o(struct base_interface* inf, unknown obj, int dis);

typedef       int      (*pf_citer_cont_dis)   (const citer from, const citer to);
typedef       void     (*pf_citer_cont_move_n)(citer itr, int n);

/* TODO: change the number */
#define CITER_CONT_INTERFACE_ID 0X00000006
#define CITER_CONT_INTERFACE_OFFSET 1
struct inf_citer_cont_vtable {
	pf_citer_cont_dis    __dis;
	pf_citer_cont_move_n __move_n;
};

#endif /* _CITER_BASE_LOCAL_H_ */
