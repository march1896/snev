#include <inf_cntr_base.h>

#include <oo_model.h>

inline void inf_cntr_base_destroy     (struct base_interface* inf) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	((struct inf_cntr_base_vtable*)(inf->__vtable))->__destroy(obj);
}

inline void inf_cntr_base_clear       (struct base_interface* inf) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	((struct inf_cntr_base_vtable*)(inf->__vtable))->__clear(obj);
}

inline int  inf_cntr_base_size        (struct base_interface* inf) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	return ((struct inf_cntr_base_vtable*)(inf->__vtable))->__size(obj);
}

inline void inf_cntr_base_add         (struct base_interface* inf, void* object) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	((struct inf_cntr_base_vtable*)(inf->__vtable))->__add(obj, object);
}

inline void inf_cntr_base_remove      (struct base_interface* inf, citer begin, citer end) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	((struct inf_cntr_base_vtable*)(inf->__vtable))->__remove(obj, begin, end);
}

inline bool inf_cntr_base_find        (struct base_interface* inf, void* object, /* out */ citer result) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	return ((struct inf_cntr_base_vtable*)(inf->__vtable))->__find(obj, object, result);
}

inline void inf_cntr_base_citer_begin (struct base_interface* inf, citer itr) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	((struct inf_cntr_base_vtable*)(inf->__vtable))->__citer_begin(obj, itr);
}

inline void inf_cntr_base_citer_end   (struct base_interface* inf, citer itr) {
	struct base_object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_INTERFACE_ID) == inf);

	((struct inf_cntr_base_vtable*)(inf->__vtable))->__citer_end(obj, itr);
}

inline void inf_cntr_base_destroy_o     (struct base_interface* inf, unknown obj) {
	((struct inf_cntr_base_vtable*)(inf->__vtable))->__destroy(obj);
}

inline void inf_cntr_base_clear_o       (struct base_interface* inf, unknown obj) {
	((struct inf_cntr_base_vtable*)(inf->__vtable))->__clear(obj);
}

inline int  inf_cntr_base_size_o        (struct base_interface* inf, unknown obj) {
	return ((struct inf_cntr_base_vtable*)(inf->__vtable))->__size(obj);
}

inline void inf_cntr_base_add_o         (struct base_interface* inf, unknown obj, void* object) {
	((struct inf_cntr_base_vtable*)(inf->__vtable))->__add(obj, object);
}

inline void inf_cntr_base_remove_o      (struct base_interface* inf, unknown obj, citer begin, citer end) {
	((struct inf_cntr_base_vtable*)(inf->__vtable))->__remove(obj, begin, end);
}

inline bool inf_cntr_base_find_o        (struct base_interface* inf, unknown obj, void* object, /* __out */citer result) {
	return ((struct inf_cntr_base_vtable*)(inf->__vtable))->__find(obj, object, result);
}

inline void inf_cntr_base_citer_begin_o (struct base_interface* inf, unknown obj, citer itr) {
	((struct inf_cntr_base_vtable*)(inf->__vtable))->__citer_begin(obj, itr);
}

inline void inf_cntr_base_citer_end_o   (struct base_interface* inf, unknown obj, citer itr) {
	((struct inf_cntr_base_vtable*)(inf->__vtable))->__citer_end(obj, itr);
}
