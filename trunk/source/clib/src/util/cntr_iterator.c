#include <cntr_iterator.h>
#include <cntr_iterator.local.h>

inline bool citer_valid(citer itr) {
	return (((cntr_iterator*)itr)->__vt)->__cvalid(itr);
};

inline void citer_to_next(citer itr) {
	(((cntr_iterator*)itr)->__vt)->__to_next(itr);
};

inline void citer_to_prev(citer itr) {
	(((cntr_iterator*)itr)->__vt)->__to_prev(itr);
};

inline void* citer_get_ref(citer itr) {
	return (((cntr_iterator*)itr)->__vt)->__get_ref(itr);
};

inline void citer_set_ref(citer itr, void* n_ref) {
	(((cntr_iterator*)itr)->__vt)->__set_ref(itr, n_ref);
};

/*
inline int citer_cntr_size(citer* itr) {
	return (((cntr_iterator*)itr)->ops)->__cntr_size(itri);
}
*/

