#ifndef _INTERFACE_DEFINITIONS_H_
#define _INTERFACE_DEFINITIONS_H_

/*
#define IFITR_ID  0X0A0B0C10
#define IBITR_ID  0X0A0B0C11
#define IRITR_ID  0X0A0B0C12
*/

#define IALLOCATOR_ID 0X0A0B0C30

#define ITR_REF_ID 0X0A0B0C21
#define ITR_ACC_ID 0X0A0B0C22
#define ITR_FWD_ID 0X0A0B0C23
#define ITR_BID_ID 0X0A0B0C24
#define ITR_RAC_ID 0X0A0B0C25

#define IOBJECT_ID 0X0A0B0C00
#define IQUEUE_ID  0X0A0B0C01
#define ISTACK_ID  0X0A0B0C02
#define ILIST_ID   0X0A0B0C03
#define ISET_ID    0X0A0B0C04
#define IMSET_ID   0X0A0B0C05
#define IMAP_ID    0X0A0B0C06
#define IMMAP_ID   0X0A0B0C07
#define IARRAY_ID  0X0A0B0C08

#include <cntr2/oo_model.h>

typedef _object* iqueue;
typedef _object* istack;
typedef _object* ilist;
typedef _object* iset;
typedef _object* imset;

typedef _object* allocator;

typedef _object* iterator;
typedef const _object* const_iterator;

typedef enum iterator_position itr_pos;

#endif /* _INTERFACE_DEFINITIONS_H_ */
