#include <cntr_iterator.h>

inline bool citer_valid(citer* itr) {
	return (itr->ops)->valid(itr->connection);
};

inline void citer_to_next(citer* itr) {
	(itr->ops)->to_next(itr->connection);
};

inline void citer_to_prev(citer* itr) {
	(itr->ops)->to_prev(itr->connection);
};

inline void* citer_ref(citer* itr) {
	return (itr->ops)->get_ref(itr->connection);
};

