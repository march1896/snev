#include <cntr2/imap.h>
#include <cntr2/iitr.h>

/*******************************************************************************
 * imap implementation start 
 ******************************************************************************/

inline void imap_clear(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	((struct imap_vtable*)(_inf->__vtable))->__clear(obj);
}
inline int imap_size(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)(_inf->__vtable))->__size(obj);
}
inline bool imap_empty(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)(_inf->__vtable))->__empty(obj);
}
inline void imap_insert(_object* obj, const unknown* key, const unknown* value) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	((struct imap_vtable*)(_inf->__vtable))->__insert(obj, key, value);
}
inline bool imap_contains(const _object* obj, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)(_inf->__vtable))->__contains(obj, key);
}
inline const unknown* imap_findbykey(const _object* obj, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)(_inf->__vtable))->__findbykey(obj, key);
}
inline void imap_remove(_object* obj, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	((struct imap_vtable*)(_inf->__vtable))->__remove(obj, key);
}
inline iterator imap_itr_create(const _object* obj, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)_inf->__vtable)->__itr_create(obj, pos);
}
inline void imap_itr_assign(const _object* obj, iterator itr, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	((struct imap_vtable*)_inf->__vtable)->__itr_assign(obj, itr, pos);
}
inline void imap_itr_find(const _object* obj, iterator itr, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	((struct imap_vtable*)(_inf->__vtable))->__itr_find(obj, itr, key);
}
inline void imap_itr_remove(_object* obj, iterator itr) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	((struct imap_vtable*)(_inf->__vtable))->__itr_remove(obj, itr);
}
inline const_iterator imap_itr_begin(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)(_inf->__vtable))->__itr_begin(obj);
}

inline const_iterator imap_itr_end(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct imap_vtable*)(_inf->__vtable))->__itr_end(obj);
}

/*******************************************************************************
 * immap implementation start 
 ******************************************************************************/

inline void immap_clear(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	((struct immap_vtable*)(_inf->__vtable))->__clear(obj);
}
inline int immap_size(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__size(obj);
}
inline bool immap_empty(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__empty(obj);
}
inline void immap_insert(_object* obj, const unknown* key, const unknown* value) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	((struct immap_vtable*)(_inf->__vtable))->__insert(obj, key, value);
}
inline bool immap_contains(const _object* obj, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__contains(obj, key);
}
inline int immap_count(const _object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__count(obj, __ref);
}
inline bool immap_remove(_object* obj, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__remove(obj, key);
}
inline iterator immap_itr_create(const _object* obj, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)_inf->__vtable)->__itr_create(obj, pos);
}
inline void immap_itr_assign(const _object* obj, iterator itr, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	((struct immap_vtable*)_inf->__vtable)->__itr_assign(obj, itr, pos);
}
inline void immap_itr_find_lower(const _object* obj, iterator itr, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	((struct immap_vtable*)(_inf->__vtable))->__itr_find_lower(obj, itr, key);
}
inline void immap_itr_find_upper(const _object* obj, iterator itr, const unknown* key) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	((struct immap_vtable*)(_inf->__vtable))->__itr_find_upper(obj, itr, key);
}
inline void immap_itr_remove(_object* obj, iterator itr) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	((struct immap_vtable*)(_inf->__vtable))->__itr_remove(obj, itr);
}
inline const_iterator immap_itr_begin(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__itr_begin(obj);
}

inline const_iterator immap_itr_end(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMMAP_ID);
	dbg_assert(_inf != NULL);

	return ((struct immap_vtable*)(_inf->__vtable))->__itr_end(obj);
}
