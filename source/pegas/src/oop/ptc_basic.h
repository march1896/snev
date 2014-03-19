#ifndef _PROTOCOL_BASIC_xECT_H_
#define _PROTOCOL_BASIC_xECT_H_

#include "oo_model2.h"

typedef unknown* (*ptc_basic_clone)   (const unknown* me, heap_info *heap);
typedef bool     (*ptc_basic_equals)  (const unknown* me, const unknown* other);
typedef int      (*ptc_basic_compare) (const unknown* me, const unknown* other);
typedef hashcode (*ptc_basic_hashcode)(const unknown* me);

struct basic_protocol {
	ptc_basic_clone    clone;
	ptc_basic_equals   equals;
	ptc_basic_compare  compare;
	ptc_basic_hashcode hashcode;
};

DECLARE_UID(basic_ptc_id);

// static dispatch protocol message to object by function.
Object  *pobject_clone(const Object *me, heap_info *heap); 
bool     pobject_equals(const Object *me, const Object *other);
int      pobject_compare(const Object *me, const Object *other);
hashcode pobject_hashcode(const Object *me);

#endif /* _PROTOCOL_BASIC_xECT_H_ */
