#include <ilist.h>

inline void ilist_destroy(iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__destroy(o);
}
inline void ilist_clear(iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__clear(o);
}
inline void ilist_clear_v(iobject* ilist, pf_ref_dispose_v dispose, void* context) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__clear_v(o, dispose, context);
}
inline int  ilist_size(const iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__size(o);
}
inline bool ilist_empty(const iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)(ilist->__vtable))->__empty(o);
}
inline const void* ilist_front(const iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)(ilist->__vtable))->__front(o);
}
inline const void* ilist_back(const iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)(ilist->__vtable))->__back(o);
}
inline void ilist_add_front(iobject* ilist, const void* __ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__add_front(o, __ref);
}
inline void ilist_add_back(iobject* ilist, const void* __ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__add_back(o, __ref);
}
inline void* ilist_remove_front(iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__remove_front(o);
}
inline void* ilist_remove_back(iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__remove_back(o);
}
inline bool ilist_contains(const iobject* ilist, const void* __ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__contains(o, __ref);
}
inline bool ilist_remove(iobject* ilist, void* __ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__remove(o, __ref);
}
inline iterator ilist_itr_create(const iobject* ilist, itr_pos pos) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__itr_create(o, pos);
}
inline void ilist_itr_assign(const iobject* ilist, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__itr_assign(o, itr, pos);
}
inline void ilist_itr_find(const iobject* ilist, iterator itr, void* ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__itr_find(o, itr, ref);
}
inline void* ilist_itr_remove(iobject* ilist, iterator itr) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__itr_remove(o, itr);
}
inline void ilist_itr_insert_before(iobject* ilist, iterator itr, void* n_ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__itr_insert_before(o, itr, n_ref);
}
inline void ilist_itr_insert_after(iobject* ilist, iterator itr, void* n_ref) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	((struct ilist_vtable*)ilist->__vtable)->__itr_insert_after(o, itr, n_ref);
}
inline const_iterator ilist_itr_begin(const iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__itr_begin(o);
}
inline const_iterator ilist_itr_end(const iobject* ilist) {
	object* o = __object_from_interface(ilist);
	dbg_assert(__cast(o, ILIST_ID) == ilist);

	return ((struct ilist_vtable*)ilist->__vtable)->__itr_end(o);
}
