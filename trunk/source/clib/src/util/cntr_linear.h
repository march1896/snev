#ifndef _CNTR_LINEAR_H_
#define _CNTR_LINEAR_H_

#include <citer_base.h>

void* cntr_front(cntr c);

void* cntr_back(cntr c);

void cntr_add_front(cntr c, void* obj);

void cntr_add_back(cntr c, void* obj);

void* cntr_remove_front(cntr c);

void* cntr_remove_back(cntr c);

#endif /* _CNTR_LINEAR_H_ */
