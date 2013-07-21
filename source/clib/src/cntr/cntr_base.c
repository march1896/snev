#include <cntr_base.h>
#include <cntr_base.local.h>

/* TODO: should we use macros ??? */
void cntr_destroy(cntr c) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	intf->__destroy(c);
}

void cntr_clear(cntr c) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	intf->__clear(c);
}

int cntr_size(cntr c) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	return intf->__size(c);
}

void cntr_add(cntr c, void* obj) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	intf->__add(c);
}

void cntr_remove(cntr c, citer begin, citer end) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	intf->__remove(c, begin, end);
}

bool cntr_find(cntr c, void* obj, citer itr) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	return intf->__find(c, obj, itr);
}

void cntr_citer_begin(cntr c, citer itr) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	intf->__citer_begin(c, itr);
}

void cntr_citer_end(cntr c, citer itr) {
	struct cntr_base_interface* intf = 
		(struct cntr_base_interface*)__cast(c, CNTR_BASE_INTERFACE);

	intf->__citer_end(c, itr);
}
