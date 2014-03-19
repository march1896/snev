#include <cntr2/iobject.h>

/* object knows how to clone itself, so alc and heap are useless */
static unknown* object_clone(const unknown *x, pf_alloc alc, void* heap) {
	_interface* _inf = (_interface*)__cast(x, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	return (unknown*)((struct iobject_vtable*)_inf->__vtable)->__clone((_object*)x);
}
/* object knows how to destroy itself, so dlc and heap are useless */
static void object_destroy(const unknown *x, pf_dealloc alc, void* heap) {
	_interface* _inf = (_interface*)__cast(x, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	((struct iobject_vtable*)_inf->__vtable)->__destroy((_object*)x);
}

static int object_compare_to(const unknown *x, const unknown *y) {
	_interface* _inf_a = (_interface*)__cast(x, IOBJECT_ID);
	_interface* _inf_b = (_interface*)__cast(y, IOBJECT_ID);

	/* first, two objects are both objects */
	if (_inf_a == NULL || _inf_b == NULL)
		rt_error("object_compare: at least one is not objects");

	/* first, two objects belongs to the same type */
	if (_inf_a->__vtable != _inf_b->__vtable)
		rt_error("object_compare: two objects are not the same type");

	/* third, the object has equals method */
	if (((struct iobject_vtable*)_inf_a->__vtable)->__compare_to == NULL)
		rt_error("object_compare: object does not support compare");

	return ((struct iobject_vtable*)_inf_a->__vtable)->__compare_to((_object*)x, (_object*)y);
}
static bool object_equals(const unknown *x, const unknown *y) {
	_interface* _inf_a = (_interface*)__cast(x, IOBJECT_ID);
	_interface* _inf_b = (_interface*)__cast(y, IOBJECT_ID);

	/* first, two objects are both objects */
	if (_inf_a == NULL || _inf_b == NULL)
		rt_error("object_equals: at least one is not objects");

	/* first, two objects belongs to the same type */
	if (_inf_a->__vtable != _inf_b->__vtable)
		rt_error("object_equals: two objects are not the same type");

	/* third, the object has equals method */
	if (((struct iobject_vtable*)_inf_a->__vtable)->__equals == NULL)
		rt_error("object_equals: object does not support equals");

	return ((struct iobject_vtable*)_inf_a->__vtable)->__equals((_object*)x, (_object*)y);
}
static hashcode object_hashcode(const unknown *x) {
	_interface* _inf = (_interface*)__cast(x, IOBJECT_ID);
	
	if (_inf == NULL) 
		rt_error("object_hashcode: reference is not object");

	if (((struct iobject_vtable*)_inf->__vtable)->__hashcode == NULL)
		rt_error("object_hascode: does not have hash function");

	return ((struct iobject_vtable*)_inf->__vtable)->__hashcode((_object*)x);
}

unknown_traits object_traits = {
	object_destroy,
	object_clone,
	object_compare_to,
	object_equals,
	object_hashcode
};

inline bool iobject_distinguishable(const _object* obj_a, const _object* obj_b) {
	_interface* _inf_a = (_interface*)__cast((unknown*)obj_a, IOBJECT_ID);
	_interface* _inf_b = (_interface*)__cast((unknown*)obj_b, IOBJECT_ID);

	/* first, two objects are both objects */
	if (_inf_a == NULL || _inf_b == NULL)
		return false;

	/* first, two objects belongs to the same type */
	if (_inf_a->__vtable != _inf_b->__vtable)
		return false;

	/* third, the object has equals method */
	if (((struct iobject_vtable*)_inf_a->__vtable)->__equals == NULL)
		return false;

	return true;
}
inline bool iobject_comparable(const _object* obj_a, const _object* obj_b) {
	_interface* _inf_a = (_interface*)__cast((unknown*)obj_a, IOBJECT_ID);
	_interface* _inf_b = (_interface*)__cast((unknown*)obj_b, IOBJECT_ID);

	/* first, two objects are both objects */
	if (_inf_a == NULL || _inf_b == NULL)
		return false;

	/* first, two objects belongs to the same type */
	if (_inf_a->__vtable != _inf_b->__vtable)
		return false;

	/* third, the object has equals method */
	if (((struct iobject_vtable*)_inf_a->__vtable)->__compare_to == NULL)
		return false;

	return true;
}
inline bool iobject_hashable(const _object* obj) {
	_interface* _inf = (_interface*)__cast((unknown*)obj, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	return ((struct iobject_vtable*)_inf->__vtable)->__hashcode != NULL;
}

inline void iobject_destroy(_object* obj) {
	_interface* _inf = (_interface*)__cast((unknown*)obj, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	((struct iobject_vtable*)_inf->__vtable)->__destroy(obj);
}
inline _object* iobject_clone(const _object* obj) {
	_interface* _inf = (_interface*)__cast((unknown*)obj, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	return ((struct iobject_vtable*)_inf->__vtable)->__clone(obj);
}
inline bool iobject_equals(const _object* obj, const _object* other) {
	_interface* _inf = (_interface*)__cast((unknown*)obj, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	return ((struct iobject_vtable*)_inf->__vtable)->__equals(obj, other);
}
inline int iobject_compare_to(const _object* obj, const _object* other) {
	_interface* _inf = (_interface*)__cast((unknown*)obj, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	return ((struct iobject_vtable*)_inf->__vtable)->__compare_to(obj, other);
}
inline hashcode iobject_hashcode(const _object* obj) {
	_interface* _inf = (_interface*)__cast((unknown*)obj, IOBJECT_ID);
	dbg_assert(_inf != NULL);

	return ((struct iobject_vtable*)_inf->__vtable)->__hashcode(obj);
}
