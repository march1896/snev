#include <cntr2/iqueue.h>
#include <cntr2/iitr.h>

inline void iqueue_clear(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	((struct iqueue_vtable*)(_inf->__vtable))->__clear(obj);
}

inline void iqueue_foreach(_object* obj, pf_ref_process_v process, void* context) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	((struct iqueue_vtable*)(_inf->__vtable))->__foreach(obj, process, context);
}

inline int  iqueue_size(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)(_inf->__vtable))->__size(obj);
}

inline bool iqueue_empty(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)(_inf->__vtable))->__empty(obj);
}

inline const unknown* iqueue_front(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)(_inf->__vtable))->__front(obj);
}

inline const unknown* iqueue_back(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)(_inf->__vtable))->__back(obj);
}

inline void iqueue_push(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	((struct iqueue_vtable*)(_inf->__vtable))->__push(obj, __ref);
}

inline void iqueue_pop(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	((struct iqueue_vtable*)(_inf->__vtable))->__pop(obj);
}
inline iterator iqueue_itr_create(const _object* obj, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)_inf->__vtable)->__itr_create(obj, pos);
}
inline void iqueue_itr_assign(const _object* obj, iterator itr, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	((struct iqueue_vtable*)_inf->__vtable)->__itr_assign(obj, itr, pos);
}

inline const_iterator iqueue_itr_begin(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)(_inf->__vtable))->__itr_begin(obj);
}

inline const_iterator iqueue_itr_end(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, IQUEUE_ID);
	dbg_assert(_inf != NULL);

	return ((struct iqueue_vtable*)(_inf->__vtable))->__itr_end(obj);
}
