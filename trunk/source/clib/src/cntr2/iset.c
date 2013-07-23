#include <iset.h>

inline void iset_destroy(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__destroy(o);
}

inline void iset_clear(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__clear(o);
}

inline int  iset_size(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__size(o);
}

inline void iset_insert(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__insert(o, ref);
}

inline iobject* iset_find(iobject* iq, void* ref) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__find(o, ref);
}

inline void iset_remove(iobject* iq, iobject* itr) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	((struct iset_vtable*)(iq->__vtable))->__remove(o, itr);
}

inline iobject* iset_itr_begin(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__itr_begin(o);
}

inline iobject* iset_itr_end(iobject* iq) {
	object* o = __object_from_interface(iq);
	dbg_assert(__cast(o, ISET_ID) == iq);

	return ((struct iset_vtable*)(iq->__vtable))->__itr_end(o);
}
