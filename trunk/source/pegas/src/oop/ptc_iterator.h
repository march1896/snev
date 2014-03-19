#ifndef _PROTOCOL_ITERATOR_H_
#define _PROTOCOL_ITERATOR_H_

#include "oo_model2.h"

DECLARE_UID(iterator_ptc_id);

typedef void           (*ptc_itr_to_begin)(unknown* itr);
typedef void           (*ptc_itr_to_end)  (unknown* itr);
typedef const unknown* (*ptc_itr_get_ref) (const unknown* itr);
typedef void           (*ptc_itr_set_ref) (unknown* itr, const unknown* __ref);
//typedef void           (*ptc_itr_swap_ref)(unknown* iterator_a, unknown* citer_b);
typedef void           (*ptc_itr_to_prev) (unknown* itr);
typedef void           (*ptc_itr_to_next) (unknown* itr);
typedef void           (*ptc_itr_advance) (unknown* itr, int length);
typedef int            (*ptc_itr_distance)(const unknown* iterator_from, const unknown* iterator_to);

typedef struct iterator_protocol_t {
	// basic protocol
	ptc_itr_to_begin  to_begin;
	ptc_itr_to_end    to_end;

	// readable protocol 
	ptc_itr_get_ref   get_ref;

	// accessible protocol
	ptc_itr_set_ref   set_ref;
	//ptc_itr_swap_ref  swap_ref;

	// forward protocol
	ptc_itr_to_next   to_next;

	// bidirectional protocol
	ptc_itr_to_prev   to_prev;

	// random accessible protocol
	ptc_itr_advance   advance;
	ptc_itr_distance  distance;
} Ptc_Iterator;

#define iterator_clone pobject_clone
#define iterator_equals pobject_equals
extern inline void           iterator_to_begin (Object* itr);
extern inline void           iterator_to_end   (Object* itr);
extern inline const unknown* iterator_get_ref  (const Object* itr);
extern inline void           iterator_set_ref  (Object* itr, const unknown* __ref);
//extern inline void           iterator_swap_ref (Object* itr, Object* other);
extern inline void           iterator_to_prev  (Object* itr);
extern inline void           iterator_to_next  (Object* itr);
extern inline void           iterator_advance  (Object* itr, int length);
extern inline int            iterator_distance (const Object* from, const Object* to);

#endif /* _PROTOCOL_ITERATOR_H_ */
