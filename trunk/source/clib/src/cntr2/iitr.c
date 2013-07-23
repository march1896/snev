#include <iitr.h>

/* forward interator interface */
inline void ifitr_destroy(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IFITR_ID) == iitr);

	(((struct ifitr_vtable*)(iitr->__vtable)))->__destroy(citr);
}

inline void ifitr_to_next(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IFITR_ID) == iitr);

	((struct ifitr_vtable*)(iitr->__vtable))->__to_next(citr);
}

inline void* ifitr_get_ref(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IFITR_ID) == iitr);

	return ((struct ifitr_vtable*)(iitr->__vtable))->__get_ref(citr);
}

inline void ifitr_set_ref(iobject* iitr, void* ref) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IFITR_ID) == iitr);

	((struct ifitr_vtable*)(iitr->__vtable))->__set_ref(citr, ref);
}

/* bidirectional iterator interface */
inline void ibitr_destroy(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IBITR_ID) == iitr);
	((struct ibitr_vtable*)(iitr->__vtable))->__destroy(citr);
}

inline void ibitr_to_prev(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IBITR_ID) == iitr);
	((struct ibitr_vtable*)(iitr->__vtable))->__to_prev(citr);
}

inline void ibitr_to_next(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IBITR_ID) == iitr);
	((struct ibitr_vtable*)(iitr->__vtable))->__to_next(citr);
}

inline void* ibitr_get_ref(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IBITR_ID) == iitr);
	return ((struct ibitr_vtable*)(iitr->__vtable))->__get_ref(citr);
}

inline void ibitr_set_ref(iobject* iitr, void* ref) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IBITR_ID) == iitr);
	((struct ibitr_vtable*)(iitr->__vtable))->__set_ref(citr, ref);
}

/* random accessed iterator interface */
inline void iritr_destroy(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IRITR_ID) == iitr);
	((struct iritr_vtable*)(iitr->__vtable))->__destroy(citr);
}

inline void iritr_to_prev(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IRITR_ID) == iitr);
	((struct iritr_vtable*)(iitr->__vtable))->__to_prev(citr);
}

inline void iritr_to_next(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IRITR_ID) == iitr);
	((struct iritr_vtable*)(iitr->__vtable))->__to_next(citr);
}

inline void* iritr_get_ref(iobject* iitr) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IRITR_ID) == iitr);
	return ((struct iritr_vtable*)(iitr->__vtable))->__get_ref(citr);
}

inline void iritr_set_ref(iobject* iitr, void* ref) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IRITR_ID) == iitr);
	((struct iritr_vtable*)(iitr->__vtable))->__set_ref(citr, ref);
}

inline void iritr_advance(iobject* iitr, int length) {
	object* citr = (object*)__object_from_interface(iitr);

	dbg_assert(__cast(citr, IRITR_ID) == iitr);
	((struct iritr_vtable*)(iitr->__vtable))->__advance(citr, length);
}

inline int  iritr_distance(iobject* ifrom, iobject* ito) {
	object* cfrom = (object*)__object_from_interface(ifrom);
	object* cto   = (object*)__object_from_interface(ito);

	dbg_assert(__cast(cfrom, IRITR_ID) == ifrom);
	dbg_assert(__cast(cto,   IRITR_ID) == ito);
	dbg_assert(ifrom->__vtable == ito->__vtable);
	return ((struct iritr_vtable*)(ifrom->__vtable))->__distance(cfrom, cto);
}
