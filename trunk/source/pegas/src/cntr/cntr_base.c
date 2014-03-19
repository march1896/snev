#include <cntr_base.h>
#include <cntr_base.local.h>

static inline cattr cntr_get_attr(cntr c) {
	return (((cntr_common_body*)c)->__vt)->__attrib(c);
}

inline bool cntr_check_attr(cntr c, cattr base) {
	return ((cntr_get_attr(c) & base) == base);
}

void cntr_destroy(cntr c) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	(cl->__vt)->__destroy(c);
}

void cntr_clear(cntr c) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	(cl->__vt)->__clear(c);
}

int cntr_size(cntr c) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	return (cl->__vt)->__size(c);
}

void cntr_add(cntr c, void* obj) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	(cl->__vt)->__add(c, obj);
}

void cntr_remove(cntr c, citer begin, citer end) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	(cl->__vt)->__remove(c, begin, end);
}

bool cntr_find(cntr c, void* obj, citer itr) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	return (cl->__vt)->__find(c, obj, itr);
}

void cntr_citer_begin(cntr c, citer itr) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	(cl->__vt)->__citer_begin(c, itr);
}

void cntr_citer_end(cntr c, citer itr) {
	cntr_common_body* cl = (cntr_common_body*)c;

	dbg_assert(cntr_check_attr(c, CNTR_ATTR_BASE));
	(cl->__vt)->__citer_end(c, itr);
}
