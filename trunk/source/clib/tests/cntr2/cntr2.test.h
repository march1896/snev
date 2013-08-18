#ifndef _CNTR2_CONTAINER_INTERFACE_TEST_H_
#define _CNTR2_CONTAINER_INTERFACE_TEST_H_
#include <cominc.h>
#include "cntr2/oo_model.h"
#include "cntr2/idef.h"

void icntr_print(iobject* icntr);

void list_correct(ilist list);
void queue_correct(iqueue queue);
void stack_correct(istack stack);
void set_correct(iset set);

#endif /* _CNTR2_CONTAINER_INTERFACE_TEST_H_ */
