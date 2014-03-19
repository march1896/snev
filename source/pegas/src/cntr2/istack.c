#include <cntr2/istack.h>
#include <cntr2/iitr.h>

inline void istack_clear(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	((struct istack_vtable*)(_inf->__vtable))->__clear(obj);
}

inline void istack_foreach(_object* obj, pf_ref_process_v process, void* context) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	((struct istack_vtable*)(_inf->__vtable))->__foreach(obj, process, context);
}

inline int istack_size(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	return ((struct istack_vtable*)(_inf->__vtable))->__size(obj);
}

inline bool istack_empty(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	return ((struct istack_vtable*)(_inf->__vtable))->__empty(obj);
}

inline const unknown* istack_top(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	return ((struct istack_vtable*)(_inf->__vtable))->__top(obj);
}

inline void istack_push(_object* obj, const unknown* __ref) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	((struct istack_vtable*)(_inf->__vtable))->__push(obj, __ref);
}

inline void istack_pop(_object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	((struct istack_vtable*)(_inf->__vtable))->__pop(obj);
}
inline iterator istack_itr_create(const _object* obj, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	return ((struct istack_vtable*)_inf->__vtable)->__itr_create(obj, pos);
}
inline void istack_itr_assign(const _object* obj, iterator itr, itr_pos pos) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	((struct istack_vtable*)_inf->__vtable)->__itr_assign(obj, itr, pos);
}
inline const_iterator istack_itr_begin(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	return ((struct istack_vtable*)(_inf->__vtable))->__itr_begin(obj);
}

inline const_iterator istack_itr_end(const _object* obj) {
	_interface* _inf = (_interface*)__cast(obj, ISTACK_ID);
	dbg_assert(_inf != NULL);

	return ((struct istack_vtable*)(_inf->__vtable))->__itr_end(obj);
}
