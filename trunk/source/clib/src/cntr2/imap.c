#include <imap.h>

/*******************************************************************************
 * imap implementation start 
 ******************************************************************************/

inline void imap_destroy(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	((struct imap_vtable*)(i->__vtable))->__destroy(o);
}

inline void imap_clear(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	((struct imap_vtable*)(i->__vtable))->__clear(o);
}

inline int imap_size(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__size(o);
}

inline bool imap_empty(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__empty(o);
}

inline void* imap_insert(iobject* i, const void* key, void* value) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__insert(o, key, value);
}

inline bool imap_contains(const iobject* i, const void* key) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__contains(o, key);
}

inline void* imap_findbykey(const iobject* i, const void* key) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__findbykey(o, key);
}

inline void* imap_remove(iobject* i, void* key) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__remove(o, key);
}
inline iterator imap_itr_create(const iobject* i, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)i->__vtable)->__itr_create(o, pos);
}
inline void imap_itr_assign(const iobject* i, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	((struct imap_vtable*)i->__vtable)->__itr_assign(o, itr, pos);
}
inline void imap_itr_find(const iobject* i, iterator itr, void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	((struct imap_vtable*)(i->__vtable))->__itr_find(o, itr, ref);
}
inline void* imap_itr_remove(iobject* i, iterator itr) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__itr_remove(o, itr);
}
inline const_iterator imap_itr_begin(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__itr_begin(o);
}

inline const_iterator imap_itr_end(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMAP_ID) == i);

	return ((struct imap_vtable*)(i->__vtable))->__itr_end(o);
}

/*******************************************************************************
 * immap implementation start 
 ******************************************************************************/
inline void immap_destroy(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	((struct immap_vtable*)(i->__vtable))->__destroy(o);
}

inline void immap_clear(iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	((struct immap_vtable*)(i->__vtable))->__clear(o);
}

inline int immap_size(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__size(o);
}

inline bool immap_empty(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__empty(o);
}

inline void immap_insert(iobject* i, const void* key, void* value) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	((struct immap_vtable*)(i->__vtable))->__insert(o, key, value);
}

inline bool immap_contains(const iobject* i, const void* key) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__contains(o, key);
}

inline int immap_count(const iobject* i, const void* __ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__count(o, __ref);
}

inline bool immap_remove(iobject* i, void* key) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__remove(o, key);
}
inline iterator immap_itr_create(const iobject* i, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)i->__vtable)->__itr_create(o, pos);
}
inline void immap_itr_assign(const iobject* i, iterator itr, itr_pos pos) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	((struct immap_vtable*)i->__vtable)->__itr_assign(o, itr, pos);
}
inline void immap_itr_find_lower(const iobject* i, iterator itr, void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	((struct immap_vtable*)(i->__vtable))->__itr_find_lower(o, itr, ref);
}
inline void immap_itr_find_upper(const iobject* i, iterator itr, void* ref) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	((struct immap_vtable*)(i->__vtable))->__itr_find_upper(o, itr, ref);
}
inline void* immap_itr_remove(iobject* i, iterator itr) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__itr_remove(o, itr);
}
inline const_iterator immap_itr_begin(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__itr_begin(o);
}

inline const_iterator immap_itr_end(const iobject* i) {
	object* o = __object_from_interface(i);
	dbg_assert(__cast(o, IMMAP_ID) == i);

	return ((struct immap_vtable*)(i->__vtable))->__itr_end(o);
}
