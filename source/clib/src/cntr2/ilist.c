#include <ilist.h>

inline void ilist_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__destroy(o);
}
inline void ilist_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__clear(o);
}
inline int  ilist_size(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__size(o);
}
inline void ilist_add_front(iobject* iq, void* __ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__add_front(o, __ref);
}
inline void ilist_add_back(iobject* iq, void* __ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__add_back(o, __ref);
}
inline void* ilist_remove_front(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__remove_front(o);
}
inline void* ilist_remove_back(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__remove_back(o);
}
inline bool ilist_contains(const iobject* iq, void* __ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__contains(o, __ref);
}
inline bool ilist_remove(iobject* iq, void* __ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__remove(o, __ref);
}
inline iterator ilist_itr_create(const iobject* iq, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_create(o, pos);
}
inline void ilist_itr_assign(const iobject* iq, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__itr_assign(o, itr, pos);
}
inline void ilist_itr_find(const iobject* iq, iterator itr, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__itr_find(o, itr, ref);
}
inline void* ilist_itr_remove(iobject* iq, iterator itr) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_remove(o, itr);
}
inline void ilist_insert_before(iobject* iq, iterator itr, void* n_ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__insert_before(o, itr, n_ref);
}
inline void ilist_insert_after(iobject* iq, iterator itr, void* n_ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__insert_after(o, itr, n_ref);
}
inline const iterator ilist_itr_begin(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_begin(o);
}
inline const iterator ilist_itr_end(const iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_end(o);
}
