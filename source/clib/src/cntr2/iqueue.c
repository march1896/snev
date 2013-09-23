#include <iqueue.h>

inline void iqueue_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)(iq->__vtable))->__destroy(o);
}

inline void iqueue_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)(iq->__vtable))->__clear(o);
}

inline int  iqueue_size(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__size(o);
}

inline bool iqueue_empty(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__empty(o);
}

inline const void* iqueue_front(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__front(o);
}

inline const void* iqueue_back(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__back(o);
}

inline void iqueue_push(iobject* iq, const void* __ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)(iq->__vtable))->__push(o, __ref);
}

inline void* iqueue_pop(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__pop(o);
}
inline iterator iqueue_itr_create(const iobject* iq, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)iq->__vtable)->__itr_create(o, pos);
}
inline void iqueue_itr_assign(const iobject* iq, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)iq->__vtable)->__itr_assign(o, itr, pos);
}

inline const_iterator iqueue_itr_begin(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__itr_begin(o);
}

inline const_iterator iqueue_itr_end(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__itr_end(o);
}
