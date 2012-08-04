#include <cntr.local.h>

void cntr_destroy(cntr c) {
	cntr_common_body* cl = (cntr_common_body*)c;

	(cl->__vt)->__destroy(cl->__cont);
	hfree(cl);
}

void cntr_clear(cntr c) {
	cntr_common_body* cl = (cntr_common_body*)c;

	(cl->__vt)->__clear(cl->__cont);
}

int cntr_size(cntr c) {
	cntr_common_body* cl = (cntr_common_body*)c;

	return (cl->__vt)->__size(cl->__cont);
}

void cntr_add(cntr c, void* obj) {
	cntr_common_body* cl = (cntr_common_body*)c;

	(cl->__vt)->__add(cl->__cont, obj);
}

void cntr_remove(cntr c, citer begin, citer end) {
	cntr_common_body* cl = (cntr_common_body*)c;

	(cl->__vt)->__remove(cl->__cont, begin, end);
}

bool cntr_find(cntr c, void* obj, citer itr) {
	cntr_common_body* cl = (cntr_common_body*)c;

	return (cl->__vt)->__find(cl->__cont, obj, itr);
}

void cntr_citer_begin(cntr c, citer itr) {
	cntr_common_body* cl = (cntr_common_body*)c;

	(cl->__vt)->__citer_begin(cl->__cont, itr);
}

void cntr_citer_end(cntr c, citer itr) {
	cntr_common_body* cl = (cntr_common_body*)c;

	(cl->__vt)->__citer_end(cl->__cont, itr);
}
