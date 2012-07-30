#include <cntr_iterator.h>

inline bool citer_valid(citer* itr) {
	return (itr->ops)->valid(itr);
};

inline bool citer_equal(citer* lhs, citer* rhs) {
	if (lhs->connection == rhs->connection) {
		dbg_assert(lhs->container == rhs->container && lhs->ops == rhs->ops);
		return true;
	}
	return false;
}

inline void citer_to_next(citer* itr) {
	(itr->ops)->to_next(itr);
};

inline void citer_to_prev(citer* itr) {
	(itr->ops)->to_prev(itr);
};

inline void* citer_get_ref(citer* itr) {
	return (itr->ops)->get_ref(itr);
};

inline void citer_set_ref(citer* itr, void* n_ref) {
	(itr->ops)->set_ref(itr, n_ref);
};

inline int citer_cntr_size(citer* itr) {
	return (itr->ops)->cntr_size(itr);
}

