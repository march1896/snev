#include <cntr_linear.h>
#include <cntr_linear.local.h>

void* cntr_front(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	dbg_assert(check_attr(c, CNTR_ATTR_BASE | CNTR_ATTR_LINEAR));
	return (void*)(cl->__vt)->__front(c);
}

void* cntr_back(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	dbg_assert(check_attr(c, CNTR_ATTR_BASE | CNTR_ATTR_LINEAR));
	return (void*)(cl->__vt)->__back(c);
}

void cntr_add_front(cntr c, void* obj) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	dbg_assert(check_attr(c, CNTR_ATTR_BASE | CNTR_ATTR_LINEAR));
	(cl->__vt)->__add_front(c, obj);
}

void cntr_add_back(cntr c, void* obj) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	dbg_assert(check_attr(c, CNTR_ATTR_BASE | CNTR_ATTR_LINEAR));
	(cl->__vt)->__add_back(c, obj);
}

void* cntr_remove_front(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	dbg_assert(check_attr(c, CNTR_ATTR_BASE | CNTR_ATTR_LINEAR));
	return (void*)(cl->__vt)->__remove_front(c);
}

void* cntr_remove_back(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	dbg_assert(check_attr(c, CNTR_ATTR_BASE | CNTR_ATTR_LINEAR));
	return (void*)(cl->__vt)->__remove_back(c);
}

