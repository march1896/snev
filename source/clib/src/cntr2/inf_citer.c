#include <inf_citer.c>

inline void inf_citer_base_to_next(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_citer_base_to_next_o(inf, obj);
}

inline void inf_citer_base_to_prev(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_citer_base_to_prev_o(inf, obj);
}

inline void* inf_citer_base_get_ref(struct base_interface* inf) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_citer_base_get_ref_o(inf, obj);
}

inline void inf_citer_base_set_ref(struct base_interface* inf, void* nref) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_citer_base_set_ref_o(inf, obj, nref);
}

inline void inf_citer_base_to_next_o(struct base_interface* inf, unknown obj) {
	return ((struct inf_citer_base_vtable*)inf->vtable)->_to_next(obj);
}

inline void inf_citer_base_to_prev_o(struct base_interface* inf, unknown obj) {
	return ((struct inf_citer_base_vtable*)inf->vtable)->_to_prev(obj);
}

inline void* inf_citer_base_get_ref_o(struct base_interface* inf, unknown obj) {
	return ((struct inf_citer_base_vtable*)inf->vtable)->_get_ref(obj);
}

inline void inf_citer_base_set_ref_o(struct base_interface* inf, unknown obj, void* nref) {
	return ((struct inf_citer_base_vtable*)inf->vtable)->_set_ref(obj, nref);
}

inline int inf_citer_cont_dis(const struct base_interface* inf_from, const struct base_interface* inf_to) {
	struct object* obj_from = (struct object*)__object_from_interface(from);
	struct object* obj_to   = (struct object*)__object_from_interface(to);

	return inf_citer_cont_dis_o(inf_from, inf_to, obj_from, obj_to);
}

inline void inf_citer_cont_move_n(struct base_interface* inf, int dis) {
	struct object* obj = (struct object*)__object_from_interface(inf);

	return inf_citer_cont_move_n_o(inf, obj);
}

inline int inf_citer_cont_dis_o(
		const struct base_interface* inf_from, 
		const struct base_interface* inf_to,
		unknown from, unknown to) {
	dbg_assert(inf_from->vtable == inf_to->vtable);
	return ((struct inf_citer_cont_vtable*)inf_from->vtable)->_dis(from, to);
}

inline void inf_citer_cont_move_n_o(struct base_interface* inf, unknown obj, int dis) {
	return ((struct inf_citer_cont_vtable*)inf->vtable)->_move_n(obj, dis);
}
