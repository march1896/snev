#include <iheap.h>

inline void iheap_join(iobject* iheap) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__join(oheap);
}
#ifdef _VERBOSE_ALLOC_DEALLOC_
inline void iheap_alloc_v(iobject* iheap, int size, const char* file, int line) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__alloc(oheap, size, file, line);
}
inline void iheap_dealloc_v(iobject* iheap, void* buff, const char* file, int line) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__dealloc(oheap, buff, file, line);
}
#else 
inline void iheap_alloc_c(iobject* iheap, int size) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__alloc(oheap, size);
}
inline void iheap_dealloc_c(iobject* iheap, void* buff) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__dealloc(oheap, buff);
}
#endif
inline void iheap_get_blockinfo(iobject* iheap, void* mem_addr, struct heap_blockinfo* binfo) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__get_blockinfo(oheap, mem_addr, binfo);
}
inline void iheap_walk(iobject* iheap, pf_process_block per_block_cb, void* param) {
	object* oheap = __object_from_interface(iheap);
	dbg_assert(__cast(oheap, IHEAP_ID) == iheap);

	((struct iheap_vtable*)iheap->__vtable)->__heap_walk(oheap, per_block_cb, param);
}
