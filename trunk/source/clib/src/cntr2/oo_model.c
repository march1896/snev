#include <oo_model.h>

inline object* __object_from_interface(const iobject* inf) {
	const iobject* inf0 = inf - (intptr_t)inf->__offset;
	object* obj = container_of(inf0, object, __iftable[0]);

	return obj;
}

inline bool __is_object(const_unknown x) {
	const object* obj = (const object*)x;
	if (obj->__offset == x) {
		//dbg_assert(obj->__cast(x, OBJECT_ME) == x);
		return true;
	}

	return false;
}

inline bool __is_interface(const_unknown x) {
	const iobject* inf = (const iobject*)x;
	dbg_assert((intptr_t)(inf->__offset) <= __MAX_NUM_INTERFACE_PER_OBJECT);

	{
		object* obj = __object_from_interface(inf);
		if (__is_object(obj)) {
			return true;
		}
	}

	return false;
}

inline unknown __cast(const_unknown x, unique_id id) {
	if (__is_object(x)) {
		object* obj = ((object*)x);
		return obj->__cast(obj, id);
	}
	else if (__is_interface(x)) {
		object* obj = (object*)__object_from_interface((iobject*)x);
		return obj->__cast(obj, id);
	}
	dbg_assert(false);

	return NULL;
}

inline iobject* __fast_cast(const object* x, int ifoffset) {
	object* obj = ((object*)x);

	dbg_assert(__is_object(obj));
	return (iobject*)&obj->__iftable[ifoffset];
}

