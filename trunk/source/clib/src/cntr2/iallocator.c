#include <iallocator.h>

inline void iallocator_join(iobject* iallocator) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	((struct iallocator_vtable*)iallocator->__vtable)->__join(oheap);
}
#ifdef _VERBOSE_ALLOC_DEALLOC_
inline void* iallocator_acquire_v(iobject* iallocator, int size, const char* file, int line) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	return ((struct iallocator_vtable*)iallocator->__vtable)->__acquire(oheap, size, file, line);
}
inline bool iallocator_release_v(iobject* iallocator, void* buff, const char* file, int line) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	return ((struct iallocator_vtable*)iallocator->__vtable)->__release(oheap, buff, file, line);
}
#else 
inline void* iallocator_acquire_c(iobject* iallocator, int size) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	return ((struct iallocator_vtable*)iallocator->__vtable)->__acquire(oheap, size);
}
inline bool iallocator_release_c(iobject* iallocator, void* buff) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	return ((struct iallocator_vtable*)iallocator->__vtable)->__release(oheap, buff);
}
#endif
inline object* iallocator_get_parent(iobject* iallocator) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	return ((struct iallocator_vtable*)iallocator->__vtable)->__get_parent(oheap);
}

inline void iallocator_walk(iobject* iallocator, pf_process_block per_block_cb, void* param) {
	object* oheap = __object_from_interface(iallocator);
	dbg_assert(__cast(oheap, IALLOCATOR_ID) == iallocator);

	((struct iallocator_vtable*)iallocator->__vtable)->__heap_walk(oheap, per_block_cb, param);
}
