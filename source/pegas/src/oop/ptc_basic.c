#include "ptc_basic.h"

DEFINE_UID(basic_ptc_id);

Object *pobject_clone(const Object *me, heap_info *heap) {
	struct basic_protocol *ptc = me->is_a->get_protocol(basic_ptc_id);

	if (ptc != NULL && ptc->clone != NULL) {
		Object *new_obj = object_create(heap);
		unknown *new_ref = ptc->clone(me->content, heap);
		object_init(new_obj, me->is_a, new_ref, heap, true);

		return new_obj;
	}
	return NULL;
}

bool pobject_equals(const Object *me, const Object *other) {
	struct basic_protocol *ptc = NULL;

	if (me->is_a != other->is_a) 
		return false;

	ptc = me->is_a->get_protocol(basic_ptc_id);

	if (ptc == NULL)
		return false;

	if (ptc->equals != NULL) {
		return ptc->equals(me->content, other->content);
	}

	if (ptc->compare != NULL) {
		return ptc->compare(me->content, other->content) == 0;
	}

	return false;
}

int pobject_compare(const Object *me, const Object *other) {
	const struct basic_protocol *ptc;

	if (me->is_a != other->is_a) 
		return false;

	ptc = me->is_a->get_protocol(basic_ptc_id);

	if (ptc != NULL && ptc->compare != NULL) {
		return ptc->compare(me->content, other->content);
	}

	return false;
}

hashcode pobject_hashcode(const Object *me) {
	struct basic_protocol *ptc = me->is_a->get_protocol(basic_ptc_id);

	if (ptc != NULL && ptc->hashcode != NULL) {
		return ptc->hashcode(me->content);
	}

	return 0;
}
