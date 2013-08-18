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
inline int  ilist_size(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__size(o);
}
inline void ilist_add_front(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__add_front(o, ref);
}
inline void ilist_add_back(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	((struct ilist_vtable*)iq->__vtable)->__add_back(o, ref);
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
inline object* ilist_itr_find(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_find(o, ref);
}
inline void* ilist_remove(iobject* iq, iterator itr) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__remove(o, itr);
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
inline object* ilist_itr_begin(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_begin(o);
}
inline object* ilist_itr_end(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ILIST_ID) == iq);

	return ((struct ilist_vtable*)iq->__vtable)->__itr_end(o);
}
