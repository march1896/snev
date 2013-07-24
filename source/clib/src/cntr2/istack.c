#include <istack.h>

inline void istack_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__destroy(o);
}

inline void istack_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__clear(o);
}

inline int  istack_size(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__size(o);
}

inline void istack_push(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	((struct istack_vtable*)(iq->__vtable))->__push(o, ref);
}

inline void* istack_pop(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__pop(o);
}

inline object* istack_itr_begin(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__itr_begin(o);
}

inline object* istack_itr_end(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISTACK_ID) == iq);

	return ((struct istack_vtable*)(iq->__vtable))->__itr_end(o);
}
