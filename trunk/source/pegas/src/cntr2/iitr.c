#include <cntr2/iitr.h>

inline unknown* itr_get_obj(const_iterator itr) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_REF_ID));

	return ((struct itr_readable_vtable*)(intf->__vtable))->__get_obj(itr);
}

inline void itr_set_obj(iterator itr, const unknown* __ref) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_ACC_ID));

	((struct itr_accessible_vtable*)(intf->__vtable))->__set_obj(itr, __ref);
}

inline const unknown* itr_get_ref(const_iterator itr) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_REF_ID));

	return ((struct itr_readable_vtable*)(intf->__vtable))->__get_ref(itr);
}

inline void itr_swap_ref(iterator itr, iterator other) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_ACC_ID));

	((struct itr_accessible_vtable*)(intf->__vtable))->__swap_ref(itr, other);
}

inline void itr_to_next(iterator itr) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_FWD_ID));

	((struct itr_forward_vtable*)(intf->__vtable))->__to_next(itr);
}

inline void itr_to_prev(iterator itr) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_BID_ID));

	((struct itr_bidirectional_vtable*)(intf->__vtable))->__to_prev(itr);
}

inline void itr_advance(iterator itr, int length) {
	_interface* intf = __fast_cast(itr, itr_interface_iterator);
	dbg_assert(intf == __cast(itr, ITR_RAC_ID));

	((struct itr_randomaccessible_vtable*)(intf->__vtable))->__advance(itr, length);
}

inline int  itr_distance(const_iterator from, const_iterator to) {
	_interface* intf = __fast_cast(from, itr_interface_iterator);
	dbg_assert(intf == __cast(from, ITR_RAC_ID));

	return ((struct itr_randomaccessible_vtable*)(intf->__vtable))->__distance(from, to);
}
