#ifndef _CITER_CONT_H_
#define _CITER_CONT_H_

#include <citer_base.h>

/*
 * Return distance between from and to, inclusive, that is if from == to, 
 * distance between them is one.
 */
extern inline int citer_dis(const citer from, const citer to);

extern inline void citer_move_n(citer itr, int dis);

#endif /* _CITER_CONT_H_ */
