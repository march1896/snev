#include <cntr_iterator.h>
#include <cntr_iterator.ph>

typedef citer cntr_iterator;

inline bool citer_valid(citer* itr) {
	cntr_iterator* ci = (cntr_iterator*)itr;

	return (ci->ops)->valid((citer*)ci);
};

inline void citer_to_next(citer* itr) {
	cntr_iterator* ci = (cntr_iterator*)itr;

	(ci->ops)->to_next((citer*)ci);
};

inline void citer_to_prev(citer* itr) {
	cntr_iterator* ci = (cntr_iterator*)itr;

	(ci->ops)->to_prev((citer*)ci);
};

inline void* citer_get_ref(citer* itr) {
	cntr_iterator* ci = (cntr_iterator*)itr;

	return (ci->ops)->get_ref((citer*)ci);
};

inline void citer_set_ref(citer* itr, void* n_ref) {
	cntr_iterator* ci = (cntr_iterator*)itr;

	(ci->ops)->set_ref((citer*)ci, n_ref);
};

inline int citer_cntr_size(citer* itr) {
	cntr_iterator* ci = (cntr_iterator*)itr;
	return (ci->ops)->cntr_size((citer*)ci);
}

inline bool citer_equal(citer* first, citer* second) {
	cntr_iterator* lhs = (cntr_iterator*) first;
	cntr_iterator* rhs = (cntr_iterator*) second;

	if (lhs->connection == rhs->connection) {
		dbg_assert(lhs->container == rhs->container && lhs->ops == rhs->ops);
		return true;
	}
	return false;
}

