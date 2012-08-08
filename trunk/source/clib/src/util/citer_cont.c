#include <citer_cont.h>
#include <citer_cont.local.h>

inline int citer_dis(const citer from, const citer to) {
	dbg_assert(citer_check_attr(from, CITER_ATTR_BASE | CITER_ATTR_CONT));

	return (((citer_cont*)from)->__vt)->__distance(from, to);
}

inline void citer_move_n(citer from, int dis) {
	dbg_assert(citer_check_attr(from, CITER_ATTR_BASE | CITER_ATTR_CONT));

	(((citer_cont*)from)->__vt)->__move_n(from, dis);
}
