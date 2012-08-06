#ifndef _CNTR_BASE_H_
#define _CNTR_BASE_H_

#include <citer_base.h>
/*
 * Methods declared in this file makes up a basic container.
 */

void cntr_destroy(cntr c);

void cntr_clear(cntr c);

int cntr_size(cntr c); 

void cntr_add(cntr c, void* obj);

void cntr_remove(cntr c, citer begin, citer end);

bool cntr_find(cntr c, void* obj, citer itr);

void cntr_citer_begin(cntr c, citer itr);

void cntr_citer_end(cntr c, citer itr);

#endif /* _CNTR_BASE_H_ */
