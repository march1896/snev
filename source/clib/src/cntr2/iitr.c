#include <iitr.h>

inline void itr_destroy(iterator itr) {
	/* iterator object always inherit iiterator as the first interface */
	iobject* intf = __fast_cast(itr, 0);
	dbg_assert(intf == __cast(itr, ITR_BAS_ID));

	((struct itr_base_vtable*)(intf->__vtable))->__destroy(itr);
}

inline bool itr_equals(const iterator ia, const iterator ib) {
	iobject* intf = __fast_cast((unknown)ia, 0);
	dbg_assert(__cast((unknown)ia, ITR_BAS_ID) == __fast_cast((unknown)ia, 0));
	dbg_assert(__cast((unknown)ib, ITR_BAS_ID) == __fast_cast((unknown)ib, 0));

	return ((struct itr_base_vtable*)(intf->__vtable))->__equals(ia, ib);
}

inline const void* itr_get_ref(const iterator itr) {
	iobject* intf = __fast_cast((unknown)itr, 0);
	dbg_assert(intf == __cast((unknown)itr, ITR_REF_ID));

	return ((struct itr_readable_vtable*)(intf->__vtable))->__get_ref(itr);
}

inline void itr_set_ref(iterator itr, const void* __ref) {
	iobject* intf = __fast_cast(itr, 0);
	dbg_assert(intf == __cast(itr, ITR_ACC_ID));

	((struct itr_accessible_vtable*)(intf->__vtable))->__set_ref(itr, __ref);
}

inline void itr_to_next(iterator itr) {
	iobject* intf = __fast_cast(itr, 0);
	dbg_assert(intf == __cast(itr, ITR_FWD_ID));

	((struct itr_forward_vtable*)(intf->__vtable))->__to_next(itr);
}

inline void itr_to_prev(iterator itr) {
	iobject* intf = __fast_cast(itr, 0);
	dbg_assert(intf == __cast(itr, ITR_BID_ID));

	((struct itr_bidirectional_vtable*)(intf->__vtable))->__to_prev(itr);
}

inline void itr_advance(iterator itr, int length) {
	iobject* intf = __fast_cast(itr, 0);
	dbg_assert(intf == __cast(itr, ITR_RAC_ID));

	((struct itr_randomaccessible_vtable*)(intf->__vtable))->__advance(itr, length);
}

inline int  itr_distance(const iterator from, const iterator to) {
	iobject* intf = __fast_cast((unknown)from, 0);
	dbg_assert(intf == __cast((unknown)from, ITR_RAC_ID));

	return ((struct itr_randomaccessible_vtable*)(intf->__vtable))->__distance(from, to);
}

/* TODO: not implemented yet */
inline void iitr_destroy(iobject* iitr);
inline bool iitr_equals(const iobject* ia, const iobject* ib);
inline void iitr_to_prev(iobject* iitr);
inline void iitr_to_next(iobject* iitr);
inline const void* iitr_get_ref(const iobject* iitr);
inline void iitr_set_ref(iobject* iitr, const void* __ref);
inline void iitr_advance(iobject* iitr, int length);
inline int  iitr_distance(const iobject* ifrom, const iobject* ito);
