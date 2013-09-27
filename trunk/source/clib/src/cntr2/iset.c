#include <iset.h>

/*******************************************************************************
 * iset implementation start 
 ******************************************************************************/

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

inline void iset_clear_v(iobject* i, pf_ref_dispose_v dispose, void* context) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	((struct iset_vtable*)(i->__vtable))->__clear_v(o, dispose, context);
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

inline void* iset_insert(iobject* i, const void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	return ((struct iset_vtable*)(i->__vtable))->__insert(o, __ref);
}

inline bool iset_contains(const iobject* i, const void* __ref) {
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
inline void iset_itr_find(const iobject* i, iterator itr, const void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, ISET_ID) == i);

	((struct iset_vtable*)(i->__vtable))->__itr_find(o, itr, __ref);
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

/*******************************************************************************
 * imset implementation start 
 ******************************************************************************/
inline void imset_destroy(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)(i->__vtable))->__destroy(o);
}

inline void imset_clear(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)(i->__vtable))->__clear(o);
}

inline void imset_clear_v(iobject* i, pf_ref_dispose_v dispose, void* context) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)(i->__vtable))->__clear_v(o, dispose, context);
}

inline int imset_size(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__size(o);
}

inline bool imset_empty(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__empty(o);
}

inline void imset_insert(iobject* i, const void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)(i->__vtable))->__insert(o, __ref);
}

inline bool imset_contains(const iobject* i, const void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__contains(o, __ref);
}

inline int imset_count(const iobject* i, const void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__count(o, __ref);
}

inline bool imset_remove(iobject* i, void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__remove(o, __ref);
}
inline iterator imset_itr_create(const iobject* i, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)i->__vtable)->__itr_create(o, pos);
}
inline void imset_itr_assign(const iobject* i, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)i->__vtable)->__itr_assign(o, itr, pos);
}
inline void imset_itr_find_lower(const iobject* i, iterator itr, const void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)(i->__vtable))->__itr_find_lower(o, itr, ref);
}
inline void imset_itr_find_upper(const iobject* i, iterator itr, const void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	((struct imset_vtable*)(i->__vtable))->__itr_find_upper(o, itr, ref);
}
inline void* imset_itr_remove(iobject* i, iterator itr) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__itr_remove(o, itr);
}
inline const_iterator imset_itr_begin(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__itr_begin(o);
}

inline const_iterator imset_itr_end(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMSET_ID) == i);

	return ((struct imset_vtable*)(i->__vtable))->__itr_end(o);
}
