#ifndef _CNTR_FACTORY_H_
#define _CNTR_FACTORY_H_

#include <citer_base.h>
#include <cntr_algorithm.h>

typedef void (*pf_preremove_cb)(void* object);

cntr cntr_create_as_list();

cntr cntr_create_as_list_r(pf_preremove_cb pre_rm);


cntr cntr_create_as_array();

cntr cntr_create_as_array_r(pf_preremove_cb pre_rm);

cntr cntr_create_as_array_v(int init_capacity, int expand_size);

cntr cntr_create_as_array_rv(pf_preremove_cb pre_rm, int init_capacity, int expand_size);


cntr cntr_create_as_bst();

cntr cntr_create_as_bst_r(pf_preremove_cb);

cntr cntr_create_as_bst_v(pf_compare_object);

cntr cntr_create_as_bst_rv(pf_preremove_cb, pf_compare_object);

#endif /* _CNTR_FACTORY_H_ */
