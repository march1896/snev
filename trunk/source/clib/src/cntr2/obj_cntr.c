#include <obj_cntr.h>

#include <inf_cntr_base.h>
#include <inf_cntr_cont.h>

void cntr_destroy     (cntr c) {
	struct base_interface* inf_base = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSE);

	inf_cntr_base_destroy_o(inf_base, c);
}

void cntr_clear       (cntr c) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_clear_o(inf, c);
}

int  cntr_size        (cntr c) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_size_o(inf, c);
}

void cntr_add         (cntr c, void* object) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_add_o(inf, c, object);
}

void cntr_remove      (cntr c, citer begin, citer end) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_remove_o(inf, c, begin, end);
}

bool cntr_find        (cntr c, void* object, /* __out */citer result) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_find_o(inf, c, object, result);
}

void cntr_citer_begin (cntr c, citer itr) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_citer_begin_o(inf, c, itr);
}

void cntr_citer_end   (cntr c, citer itr) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_BASE_INTERFACE_OFFSET);

	inf_cntr_base_citer_end_o(inf, c, itr);
}

void* cntr_front      (cntr c) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_LINEAR_INTERFACE_OFFSET);

	inf_cntr_linear_front_o(inf, c); 
}

void* cntr_back       (cntr c) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_LINEAR_INTERFACE_OFFSET);

	inf_cntr_linear_back_o(inf, c); 
}

void  cntr_add_front  (cntr c, void* object) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_LINEAR_INTERFACE_OFFSET);

	inf_cntr_linear_add_front_o(inf, c, object); 
}

void  cntr_add_back   (cntr c, void* object) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_LINEAR_INTERFACE_OFFSET);

	inf_cntr_linear_add_back_o(inf, c, object); 
}

void* cntr_remove_front(cntr c) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_LINEAR_INTERFACE_OFFSET);

	inf_cntr_linear_remove_front_o(inf, c); 
}

void* cntr_remove_back(cntr c) {
	struct base_interface* inf = 
		(struct base_interface*)__fast_cast(c, CNTR_LINEAR_INTERFACE_OFFSET);

	inf_cntr_linear_remove_back(inf, c); 
}

#endif /* _OBJECT_CNTR_H_ */
