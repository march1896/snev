#include "ptc_iterator.h"

DEFINE_UID(iterator_ptc_id);

inline void iterator_to_begin(Object* me) {
	Ptc_Iterator *ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	ptc->to_begin(me->content);
}

inline void iterator_to_end(Object* me) {
	Ptc_Iterator *ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	ptc->to_end(me->content);
}

inline const unknown* iterator_get_ref(const Object* me) {
	Ptc_Iterator* ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	return ptc->get_ref(me->content);
}

inline void iterator_set_ref(Object* me, const unknown* __ref) {
	Ptc_Iterator* ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	ptc->set_ref(me->content, __ref);
}

// inline void iterator_swap_ref(Object* me, Object* other) {
// 	struct iterator_protocol* ptc = me->is_a->get_protocol(iterator_ptc_id);
// 	dbg_assert(ptc != NULL);
// 
// 	ptc->swap_ref(me, other);
// }

inline void iterator_to_next(Object* me) {
	Ptc_Iterator* ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	ptc->to_next(me->content);
}

inline void iterator_to_prev(Object* me) {
	Ptc_Iterator* ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	ptc->to_prev(me->content);
}

inline void iterator_advance(Object* me, int length) {
	Ptc_Iterator* ptc = (Ptc_Iterator*)me->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);

	ptc->advance(me->content, length);
}

// double dispatch
inline int iterator_distance(const Object* from, const Object* to) {
	Ptc_Iterator* ptc = (Ptc_Iterator*)from->is_a->get_protocol(iterator_ptc_id);
	dbg_assert(ptc != NULL);
	dbg_assert(from->is_a == to->is_a);

	return ptc->distance(from->content, to->content);
}
