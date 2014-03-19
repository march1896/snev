#include <cntr2/iset.h>
#include <cntr2/iitr.h>

/*******************************************************************************
 * iset implementation start 
 ******************************************************************************/

inline void iset_clear(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	((struct iset_vtable*)(_inf->__vtable))->__clear(obj);
}

inline int iset_size(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__size(obj);
}

inline bool iset_empty(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__empty(obj);
}

inline void iset_insert(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	((struct iset_vtable*)(_inf->__vtable))->__insert(obj, __ref);
}

inline void iset_replace(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	((struct iset_vtable*)(_inf->__vtable))->__insert(obj, __ref);
}

inline bool iset_contains(const _object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__contains(obj, __ref);
}

inline bool iset_remove(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__remove(obj, __ref);
}
inline iterator iset_itr_create(const _object* obj, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__itr_create(obj, pos);
}
inline void iset_itr_assign(const _object* obj, iterator itr, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	((struct iset_vtable*)(_inf->__vtable))->__itr_assign(obj, itr, pos);
}
inline void iset_itr_find(const _object* obj, iterator itr, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	((struct iset_vtable*)(_inf->__vtable))->__itr_find(obj, itr, __ref);
}
inline void iset_itr_remove(_object* obj, iterator itr) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	((struct iset_vtable*)(_inf->__vtable))->__itr_remove(obj, itr);
}
inline const_iterator iset_itr_begin(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__itr_begin(obj);
}

inline const_iterator iset_itr_end(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISET_ID);
	dbg_assert(_inf != NULL);

	return ((struct iset_vtable*)(_inf->__vtable))->__itr_end(obj);
}

/*******************************************************************************
 * imset implementation start 
 ******************************************************************************/

inline void imset_clear(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	((struct imset_vtable*)(_inf->__vtable))->__clear(obj);
}

inline int imset_size(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__size(obj);
}

inline bool imset_empty(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__empty(obj);
}

inline void imset_insert(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	((struct imset_vtable*)(_inf->__vtable))->__insert(obj, __ref);
}

inline bool imset_contains(const _object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__contains(obj, __ref);
}

inline int imset_count(const _object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__count(obj, __ref);
}

inline int imset_remove(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__remove(obj, __ref);
}
inline iterator imset_itr_create(const _object* obj, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__itr_create(obj, pos);
}
inline void imset_itr_assign(const _object* obj, iterator itr, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	((struct imset_vtable*)(_inf->__vtable))->__itr_assign(obj, itr, pos);
}
inline void imset_itr_find_lower(const _object* obj, iterator itr, const unknown* ref) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	((struct imset_vtable*)(_inf->__vtable))->__itr_find_lower(obj, itr, ref);
}
inline void imset_itr_find_upper(const _object* obj, iterator itr, const unknown* ref) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	((struct imset_vtable*)(_inf->__vtable))->__itr_find_upper(obj, itr, ref);
}
inline void imset_itr_remove(_object* obj, iterator itr) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	((struct imset_vtable*)(_inf->__vtable))->__itr_remove(obj, itr);
}
inline const_iterator imset_itr_begin(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__itr_begin(obj);
}

inline const_iterator imset_itr_end(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IMSET_ID);
	dbg_assert(_inf != NULL);

	return ((struct imset_vtable*)(_inf->__vtable))->__itr_end(obj);
}
