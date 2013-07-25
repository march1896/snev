#include <iitr.h>

inline void itr_destroy(object* itr) {
	iobject* intf = __fast_cast(itr, 0);

	((struct itr_base_vtable*)(intf->__vtable))->__destroy(itr);
}

inline bool itr_equals(object* ia, object* ib) {
	iobject* intf = __fast_cast(ia, 0);

	return ((struct itr_base_vtable*)(intf->__vtable))->__equals(ia, ib);
}

inline void* itr_get_ref(object* itr) {
	iobject* intf = __fast_cast(itr, 0);

	return ((struct itr_readable_vtable*)(intf->__vtable))->__get_ref(itr);
}

inline void itr_set_ref(object* itr, void* __ref) {
	iobject* intf = __fast_cast(itr, 0);

	((struct itr_accessible_vtable*)(intf->__vtable))->__set_ref(itr, __ref);
}

inline void itr_to_next(object* itr) {
	iobject* intf = __fast_cast(itr, 0);

	((struct itr_forward_vtable*)(intf->__vtable))->__to_next(itr);
}

inline void itr_to_prev(object* itr) {
	iobject* intf = __fast_cast(itr, 0);

	((struct itr_bidirectional_vtable*)(intf->__vtable))->__to_prev(itr);
}

inline void itr_advance(object* itr, int length) {
	iobject* intf = __fast_cast(itr, 0);

	((struct itr_randomaccessible_vtable*)(intf->__vtable))->__advance(itr, length);
}

inline int  itr_distance(object* from, object* to) {
	iobject* intf = __fast_cast(from, 0);

	return ((struct itr_randomaccessible_vtable*)(intf->__vtable))->__distance(from, to);
}

/* TODO: not implemented yet */
inline void iitr_destroy(iobject* iitr);
inline bool iitr_equals(iobject* ia, iobject* ib);
inline void iitr_to_prev(iobject* iitr);
inline void iitr_to_next(iobject* iitr);
inline void* iitr_get_ref(iobject* iitr);
inline void iitr_set_ref(iobject* iitr, void* __ref);
inline void iitr_advance(iobject* iitr, int length);
inline int  iitr_distance(iobject* ifrom, iobject* ito);
