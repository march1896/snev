#ifndef _OBJECT_CITER_H_
#define _OBJECT_CITER_H_

#include <cntr_def.h>

extern inline void     citer_to_next(citer itr);

extern inline void     citer_to_prev(citer itr);

extern inline void*    citer_get_ref(citer itr);

extern inline void     citer_set_ref(citer itr, void* nref);

/* Return distance between from and to, inclusive, so, if from == to, distance is one. */
extern inline int      citer_dis(const citer from, const citer to);

extern inline void     citer_move_n(citer itr, int dis);

#include <oo_model.h>
struct citer_t {
	address                   __offset;
	pf_cast                   __cast;
	struct base_interface     __iftable[2];

	void*                     position;
	void*                     param_x;
	void*                     param_y;
	void*                     param_z;
};

//extern inline void     citer_init(citer itr, 

#endif /* _OBJECT_CITER_H_ */
