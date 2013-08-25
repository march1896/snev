#include <iset.h>

inline void iset_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__destroy(o);
}

inline void iset_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__clear(o);
}

inline int iset_size(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__size(o);
}

inline bool iset_empty(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__empty(o);
}

inline bool iset_insert(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__insert(o, ref);
}

inline bool iset_contains(const iobject* ic, void* __ref) {
	object* o = __object_from_interface(ic);
	dbg_assert(__cast(o, ISET_ID) == ic);

	return ((struct iset_vtable*)(ic->__vtable))->__contains(o, __ref);
}

inline bool iset_remove(iobject* ic, void* __ref) {
	object* o = __object_from_interface(ic);
	dbg_assert(__cast(o, ISET_ID) == ic);

	return ((struct iset_vtable*)(ic->__vtable))->__remove(o, __ref);
}
inline iterator iset_itr_create(const iobject* iq, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)iq->__vtable)->__itr_create(o, pos);
}
inline void iset_itr_assign(const iobject* iq, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)iq->__vtable)->__itr_assign(o, itr, pos);
}
inline void iset_itr_find(const iobject* iq, iterator itr, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__itr_find(o, itr, ref);
}
inline void* iset_itr_remove(iobject* iq, iterator itr) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__itr_remove(o, itr);
}
inline const_iterator iset_itr_begin(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__itr_begin(o);
}

inline const_iterator iset_itr_end(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__itr_end(o);
}
