#include <cntr_base_if.h>

#include <oo_model.h>

inline void inf_cntr_base_destroy     (struct base_interface* inf) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__destroy(obj);
}

inline void inf_cntr_base_clear       (struct base_interface* inf) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__clear(obj);
}

inline int  inf_cntr_base_size        (struct base_interface* inf) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__size(obj);
}

inline void inf_cntr_base_add         (struct base_interface* inf, void* object) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__add(obj, object);
}

inline void inf_cntr_base_remove      (struct base_interface* inf, citer begin, citer end) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__remove(obj, begin, end);
}

inline bool inf_cntr_base_find        (struct base_interface* inf, void* object, /* out */ citer result) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__find(obj, object, result);
}

inline void inf_cntr_base_citer_begin (struct base_interface* inf, citer itr) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__begin(obj, itr);
}

inline void inf_cntr_base_citer_end   (struct base_interface* inf, citer itr) {
	struct object* obj = __object_from_interface(inf);
	dbg_assert(__cast(inf, CNTR_BASE_IF) == inf);

	return inf->__vtable->__citer_end(obj, itr);
}

inline void inf_cntr_base_destroy_o     (struct base_interface* inf, unknown obj) {
	return inf->__vtable->_destroy(obj);
}

inline void inf_cntr_base_clear_o       (struct base_interface* inf, unknown obj) {
	return inf->__vtable->_clear(obj);
}

inline int  inf_cntr_base_size_o        (struct base_interface* inf, unknown obj) {
	return inf->__vtable->_size(obj);
}

inline void inf_cntr_base_add_o         (struct base_interface* inf, unknown obj, void* object) {
	return inf->__vtable->_add(obj, object);
}

inline void inf_cntr_base_remove_o      (struct base_interface* inf, unknown obj, citer begin, citer end) {
	return inf->__vtable->_remove(obj, begin, end);
}

inline bool inf_cntr_base_find_o        (struct base_interface* inf, unknown obj, void* object, /* __out */citer result) {
	return inf->__vtable->_find(obj, object, result);
}

inline void inf_cntr_base_citer_begin_o (struct base_interface* inf, unknown obj, citer itr) {
	return inf->__vtable->_citer(obj, itr);
}

inline void inf_cntr_base_citer_end_o   (struct base_interface* inf, unknown obj, citer itr) {
	return inf->__vtable->_citer(obj, itr);
}
