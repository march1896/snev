#include <istack.h>

inline void istack_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__destroy(o);
}

inline void istack_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__clear(o);
}

inline void istack_clear_v(iobject* iq, pf_ref_dispose_v dispose, void* context) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__clear_v(o, dispose, context);
}

inline int istack_size(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__size(o);
}

inline bool istack_empty(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__empty(o);
}

inline const void* istack_top(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__top(o);
}

inline void istack_push(iobject* iq, const void* __ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__push(o, __ref);
}

inline void* istack_pop(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__pop(o);
}
inline iterator istack_itr_create(const iobject* iq, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)iq->__vtable)->__itr_create(o, pos);
}
inline void istack_itr_assign(const iobject* iq, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)iq->__vtable)->__itr_assign(o, itr, pos);
}
inline const_iterator istack_itr_begin(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__itr_begin(o);
}

inline const_iterator istack_itr_end(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__itr_end(o);
}
