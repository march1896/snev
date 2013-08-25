#include <iset.h>

inline void iset_destroy(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	((struct iset_vtable*)(i->__vtable))->__destroy(o);
}

inline void iset_clear(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	((struct iset_vtable*)(i->__vtable))->__clear(o);
}

inline int iset_size(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__size(o);
}

inline bool iset_empty(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__empty(o);
}

inline bool iset_insert(iobject* i, void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__insert(o, ref);
}

inline bool iset_contains(const iobject* i, void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__contains(o, __ref);
}

inline bool iset_remove(iobject* i, void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__remove(o, __ref);
}
inline iterator iset_itr_create(const iobject* i, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)i->__vtable)->__itr_create(o, pos);
}
inline void iset_itr_assign(const iobject* i, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	((struct iset_vtable*)i->__vtable)->__itr_assign(o, itr, pos);
}
inline void iset_itr_find(const iobject* i, iterator itr, void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	((struct iset_vtable*)(i->__vtable))->__itr_find(o, itr, ref);
}
inline void* iset_itr_remove(iobject* i, iterator itr) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__itr_remove(o, itr);
}
inline const_iterator iset_itr_begin(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__itr_begin(o);
}

inline const_iterator iset_itr_end(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__itr_end(o);
}
