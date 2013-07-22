#include <inf_cntr_linear.h>
#include <oo_model.h>

extern inline void* inf_cntr_linear_front       (struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_cntr_linear_front_o(inf, obj);
}

extern inline void* inf_cntr_linear_back        (struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_cntr_linear_back_o(inf, obj);
}

extern inline void  inf_cntr_linear_add_front   (struct base_interface* inf, void* object) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_add_front_o(inf, obj, object);
}

extern inline void  inf_cntr_linear_add_back    (struct base_interface* inf, void* object) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_add_back_o(inf, obj, object);
}

extern inline void* inf_cntr_linear_remove_front(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_cntr_linear_remove_front_o(inf, obj);
}

extern inline void* inf_cntr_linear_remove_back (struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_cntr_linear_remove_back_o(inf, obj);
}

extern inline void* inf_cntr_linear_front_o       (struct base_interface* inf, unknown obj) {
	return ((struct inf_cntr_linear_vtable*)inf->__vtable)->__front(obj);
}
extern inline void* inf_cntr_linear_back_o        (struct base_interface* inf, unknown obj) {
	return ((struct inf_cntr_linear_vtable*)inf->__vtable)->__back(obj);
}
extern inline void  inf_cntr_linear_add_front_o   (struct base_interface* inf, unknown obj, void* object) {
	((struct inf_cntr_linear_vtable*)inf->__vtable)->__add_front(obj, object);
}
extern inline void  inf_cntr_linear_add_back_o    (struct base_interface* inf, unknown obj, void* object) {
	((struct inf_cntr_linear_vtable*)inf->__vtable)->__add_back(obj, object);
}
extern inline void* inf_cntr_linear_remove_front_o(struct base_interface* inf, unknown obj) {
	return ((struct inf_cntr_linear_vtable*)inf->__vtable)->__remove_front(obj);
}
extern inline void* inf_cntr_linear_remove_back_o (struct base_interface* inf, unknown obj) {
	return ((struct inf_cntr_linear_vtable*)inf->__vtable)->__remove_back(obj);
}

