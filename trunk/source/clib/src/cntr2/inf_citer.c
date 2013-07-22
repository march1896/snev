#include <inf_citer.h>

#include <oo_model.h>

inline void inf_citer_base_to_next(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_citer_base_to_next_o(inf, obj);
}

inline void inf_citer_base_to_prev(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_citer_base_to_prev_o(inf, obj);
}

inline void* inf_citer_base_get_ref(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_citer_base_get_ref_o(inf, obj);
}

inline void inf_citer_base_set_ref(struct base_interface* inf, void* nref) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_citer_base_set_ref_o(inf, obj, nref);
}

inline void inf_citer_base_to_next_o(struct base_interface* inf, unknown obj) {
	((struct inf_citer_base_vtable*)inf->__vtable)->__to_next(obj);
}

inline void inf_citer_base_to_prev_o(struct base_interface* inf, unknown obj) {
	((struct inf_citer_base_vtable*)inf->__vtable)->__to_prev(obj);
}

inline void* inf_citer_base_get_ref_o(struct base_interface* inf, unknown obj) {
	return ((struct inf_citer_base_vtable*)inf->__vtable)->__get_ref(obj);
}

inline void inf_citer_base_set_ref_o(struct base_interface* inf, unknown obj, void* nref) {
	((struct inf_citer_base_vtable*)inf->__vtable)->__set_ref(obj, nref);
}

inline int inf_citer_cont_dis(const struct base_interface* inf_from, const struct base_interface* inf_to) {
	struct object* obj_from = (struct object*)__object_from_interface(inf_from);
	struct object* obj_to   = (struct object*)__object_from_interface(inf_to);

	return inf_citer_cont_dis_o(inf_from, inf_to, (unknown)obj_from, (unknown)obj_to);
}

inline void inf_citer_cont_move_n(struct base_interface* inf, int dis) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	inf_citer_cont_move_n_o(inf, obj, dis);
}

inline int inf_citer_cont_dis_o(
		const struct base_interface* inf_from, 
		const struct base_interface* inf_to,
		unknown from, unknown to) {
	dbg_assert(inf_from->__vtable == inf_to->__vtable);
	return ((struct inf_citer_cont_vtable*)inf_from->__vtable)->__dis(from, to);
}

inline void inf_citer_cont_move_n_o(struct base_interface* inf, unknown obj, int dis) {
	((struct inf_citer_cont_vtable*)inf->__vtable)->__move_n(obj, dis);
}
