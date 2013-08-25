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
#define ILIST_ID  0X0A0B0C03
#define ISET_ID   0X0A0B0C04
#define IMSET_ID  0X0A0B0C05

#include <oo_model.h>

typedef iobject* iqueue;
typedef iobject* istack;
typedef iobject* ilist;
typedef iobject* iset;
typedef iobject* imset;

typedef object* allocator;

typedef object* iterator;
/* 'const iterator' equals 'object* const', so we should have a new type */
typedef const object* const_iterator;

typedef enum {
	itr_begin,
	itr_end,
	itr_pos_count
} itr_pos;

#endif /* _INTERFACE_DEFINITIONS_H_ */
