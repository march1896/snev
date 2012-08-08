#ifndef _CNTR_FACTORY_H_
#define _CNTR_FACTORY_H_

#include <citer_base.h>
#include <cntr_algorithm.h>

cntr cntr_create_as_list();
cntr cntr_create_as_array();
cntr cntr_create_as_array_v(int init_capacity, int expand_size);
cntr cntr_create_as_bst(pf_compare_object);

#endif /* _CNTR_FACTORY_H_ */
