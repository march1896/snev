#include <iqueue.h>

inline void iqueue_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)(iq->__vtable))->__destroy(o);
}

inline void iqueue_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)(iq->__vtable))->__clear(o);
}

inline int  iqueue_size(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__size(o);
}

inline void iqueue_push(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	((struct iqueue_vtable*)(iq->__vtable))->__push(o, ref);
}

inline void* iqueue_pop(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__pop(o);
}

inline object* iqueue_itr_begin(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__itr_begin(o);
}

inline object* iqueue_itr_end(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, IQUEUE_ID) == iq);

	return ((struct iqueue_vtable*)(iq->__vtable))->__itr_end(o);
}
