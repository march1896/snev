#ifndef _INTERFACE_DEFINITIONS_H_
#define _INTERFACE_DEFINITIONS_H_

/*
#define IFITR_ID  0X0A0B0C10
#define IBITR_ID  0X0A0B0C11
#define IRITR_ID  0X0A0B0C12
*/

#define IALLOCATOR_ID 0X0A0B0C30

#define ITR_BAS_ID 0X0A0B0C20
#define ITR_REF_ID 0X0A0B0C21
#define ITR_ACC_ID 0X0A0B0C22
#define ITR_FWD_ID 0X0A0B0C23
#define ITR_BID_ID 0X0A0B0C24
#define ITR_RAC_ID 0X0A0B0C25

#define IQUEUE_ID 0X0A0B0C01
#define ISTACK_ID 0X0A0B0C02
#define ISET_ID   0X0A0B0C03
#define ILIST_ID  0X0a0B0C04

#include <oo_model.h>

typedef iobject* iqueue;
typedef iobject* istack;
typedef iobject* ilist;
typedef iobject* iset;

typedef object* allocator;

typedef object* container;
typedef object* iterator;
typedef object* itrbas;
typedef object* itrref;
typedef object* itracc;
typedef object* itrfwd;
typedef object* itrbid;
typedef object* itrrac;

#endif /* _INTERFACE_DEFINITIONS_H_ */
