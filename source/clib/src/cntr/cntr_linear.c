#include <cntr_linear.h>
#include <cntr_linear.local.h>

void* cntr_front(cntr c) {
	struct cntr_linear_interface* intf = 
		(struct cntr_linear_interface*)__cast(c, CNTR_LINEAR_INTERFACE);

	return intf->__front(c);
}

void* cntr_back(cntr c) {
	struct cntr_linear_interface* intf = 
		(struct cntr_linear_interface*)__cast(c, CNTR_LINEAR_INTERFACE);

	return intf->__back(c);
}

void cntr_add_front(cntr c, void* obj) {
	struct cntr_linear_interface* intf = 
		(struct cntr_linear_interface*)__cast(c, CNTR_LINEAR_INTERFACE);

	return intf->__add_front(c, obj);
}

void cntr_add_back(cntr c, void* obj) {
	struct cntr_linear_interface* intf = 
		(struct cntr_linear_interface*)__cast(c, CNTR_LINEAR_INTERFACE);

	return intf->__add_back(c, obj);
}

void* cntr_remove_front(cntr c) {
	struct cntr_linear_interface* intf = 
		(struct cntr_linear_interface*)__cast(c, CNTR_LINEAR_INTERFACE);

	return intf->__remove_front(c);
}

void* cntr_remove_back(cntr c) {
	struct cntr_linear_interface* intf = 
		(struct cntr_linear_interface*)__cast(c, CNTR_LINEAR_INTERFACE);

	return intf->__remove_back(c);
}

