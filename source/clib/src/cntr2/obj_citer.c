#include <obj_citer.h>

inline void citer_to_next(citer itr) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(itr, CITER_BASE_INTERFACE_OFFSET);

	return inf_citer_base_to_next_o(inf, itr);
}

inline void citer_to_prev(citer itr) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(itr, CITER_BASE_INTERFACE_OFFSET);

	return inf_citer_base_to_prev_o(inf, itr);
}

inline void* citer_get_ref(citer itr) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(itr, CITER_BASE_INTERFACE_OFFSET);

	return inf_citer_base_get_ref_o(inf, itr);
}

inline void citer_set_ref(citer itr, void* nref) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(itr, CITER_BASE_INTERFACE_OFFSET);

	return inf_citer_base_set_ref_o(inf, itr, nref);
}

inline int citer_dis(const citer from, const citer to) {
	struct base_interface* inf_from = 
		(struct base_interface*)__fast_cast(from, CITER_CONT_INTERFACE_OFFSET);

	struct base_interface* inf_to = 
		(struct base_interface*)__fast_cast(to, CITER_CONT_INTERFACE_OFFSET);

	return inf_citer_base_dis_o(inf_from, itr_to);
}

inline void citer_move_n(citer itr, int dis) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(itr, CITER_CONT_INTERFACE_OFFSET);

	return inf_citer_base_move_n_o(inf, itr, dis);
}
