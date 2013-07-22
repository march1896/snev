#include <inf_cntr_linear.h>
#include <oo_model.h>

extern inline void* inf_cntr_linear_front       (struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_front_o(inf, obj);
}

extern inline void* inf_cntr_linear_back        (struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_back_o(inf, obj);
}

extern inline void  inf_cntr_linear_add_front   (struct base_interface* inf, void* object) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_add_front_o(inf, obj);
}

extern inline void  inf_cntr_linear_add_back    (struct base_interface* inf, void* object) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_add_back_o(inf, obj);
}

extern inline void* inf_cntr_linear_remove_front(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_remove_front_o(inf, obj);
}

extern inline void* inf_cntr_linear_remove_back (struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_cntr_linear_remove_back_o(inf, obj);
}

extern inline void* inf_cntr_linear_front_o       (struct base_interface* inf, unknown obj) {
	((struct cntr_linear_vtable*)inf->__vtable)->_front(obj);
}
extern inline void* inf_cntr_linear_back_o        (struct base_interface* inf, unknown obj) {
	((struct cntr_linear_vtable*)inf->__vtable)->_back(obj);
}
extern inline void  inf_cntr_linear_add_front_o   (struct base_interface* inf, unknown obj, void* object) {
	((struct cntr_linear_vtable*)inf->__vtable)->_add(obj);
}
extern inline void  inf_cntr_linear_add_back_o    (struct base_interface* inf, unknown obj, void* object) {
	((struct cntr_linear_vtable*)inf->__vtable)->_add(obj);
}
extern inline void* inf_cntr_linear_remove_front_o(struct base_interface* inf, unknown obj) {
	((struct cntr_linear_vtable*)inf->__vtable)->_remove(obj);
}
extern inline void* inf_cntr_linear_remove_back_o (struct base_interface* inf, unknown obj) {
	((struct cntr_linear_vtable*)inf->__vtable)->_remove(obj);
}

