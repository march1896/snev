#include <oo_model.h>

inline struct base_object* __object_from_interface(const struct base_interface* inf) {
	const struct base_interface* inf0 = inf - (intptr_t)inf->__offset;
	struct base_object* obj = container_of(inf0, struct base_object, __iftable[0]);

	return obj;
}

inline bool __is_object(const unknown x) {
	struct base_object* obj = (struct base_object*)x;
	if (obj->__offset == __address_of(x)) {
		//dbg_assert(obj->__cast(x, OBJECT_ME) == x);
		return true;
	}

	return false;
}

inline bool __is_interface(const unknown x) {
	struct base_interface* inf = (struct base_interface*)x;
	dbg_assert((intptr_t)(inf->__offset) <= __MAX_NUM_INTERFACE_PER_OBJECT);

	{
		struct base_object* obj = __object_from_interface(inf);
		if (__is_object(obj)) {
			return true;
		}
	}

	return false;
}

inline unknown __cast(unknown x, unique_id id) {
	if (__is_object(x)) {
		struct base_object* obj = ((struct base_object*)x);
		return obj->__cast(obj, id);
	}
	else if (__is_interface(x)) {
		struct base_object* obj = (struct base_object*)__object_from_interface((struct base_interface*)x);
		return obj->__cast(obj, id);
	}
	dbg_assert(false);

	return NULL;
}

inline struct base_interface* __fast_cast(const unknown x, int ifoffset) {
	struct base_object* obj = ((struct base_object*)x);

	dbg_assert(__is_object(obj));
	return (struct base_interface*)&obj->__iftable[ifoffset];
}
