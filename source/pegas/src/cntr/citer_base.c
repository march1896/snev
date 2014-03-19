#include <citer_base.h>
#include <citer_base.local.h>

static inline cattr citer_get_attr(citer c) {
	return (((citer_base*)c)->__vt)->__attrib(c);
}

inline bool citer_check_attr(citer c, cattr base) {
	return ((citer_get_attr(c) & base) == base);
}

inline cattr citer_attrib(citer itr) {
	dbg_assert(citer_check_attr(itr, CITER_ATTR_BASE));

	return (((citer_base*)itr)->__vt)->__attrib(itr);
};

inline void citer_to_next(citer itr) {
	dbg_assert(citer_check_attr(itr, CITER_ATTR_BASE));

	(((citer_base*)itr)->__vt)->__to_next(itr);
};

inline void citer_to_prev(citer itr) {
	dbg_assert(citer_check_attr(itr, CITER_ATTR_BASE));

	(((citer_base*)itr)->__vt)->__to_prev(itr);
};

inline void* citer_get_ref(citer itr) {
	dbg_assert(citer_check_attr(itr, CITER_ATTR_BASE));

	return (((citer_base*)itr)->__vt)->__get_ref(itr);
};

inline void citer_set_ref(citer itr, void* n_ref) {
	dbg_assert(citer_check_attr(itr, CITER_ATTR_BASE));

	(((citer_base*)itr)->__vt)->__set_ref(itr, n_ref);
};

