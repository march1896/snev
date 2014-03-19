#include "nfa.h"
#include "automaton.h"

#include <cntr2/ilist.h>
#include <cntr2/ialgo.h>

static void add_state(const void* __state, void* param) {
	atm* a = (nfa*)param;
	ilist_add_back(a->states, __state);
}

nfa* nfa_concat(nfa* first, nfa* second) {
	atm_context* context = first->context;
	atm* fa = atm_create(context);
	atm_state *source, *dest;

	dbg_assert(first->context == second->context);
	dbg_assert(ilist_size(first->accept_states) == 1);
	dbg_assert(ilist_size(second->accept_states) == 1);

	foreach_v(ilist_itr_begin(first->states), ilist_itr_end(first->states), add_state, fa);
	foreach_v(ilist_itr_begin(second->states), ilist_itr_end(second->states), add_state, fa);
	source = atm_state_create(fa);
	dest   = atm_state_create(fa);
	dbg_assert(source != NULL && dest != NULL);

	fa->start_state = source;
	ilist_add_back(fa->accept_states, dest);

	atm_transform_create(fa, source, first->start_state, Epsilon);
	atm_transform_create(fa, (atm_state*)ilist_front(first->accept_states), second->start_state, Epsilon);
	atm_transform_create(fa, (atm_state*)ilist_front(second->accept_states), dest, Epsilon);

	first->lifestate = atm_joined;
	second->lifestate = atm_joined;
	atm_destroy(first);
	atm_destroy(second);

	fa->lifestate = atm_active;
	return fa;
}

nfa* nfa_union (nfa* first, nfa* second) {
	atm_context* context = first->context;
	atm* fa = atm_create(context);
	atm_state *source, *dest;

	dbg_assert(first->context == second->context);
	dbg_assert(ilist_size(first->accept_states) == 1);
	dbg_assert(ilist_size(second->accept_states) == 1);

	foreach_v(ilist_itr_begin(first->states), ilist_itr_end(first->states), add_state, fa);
	foreach_v(ilist_itr_begin(second->states), ilist_itr_end(second->states), add_state, fa);
	source = atm_state_create(fa);
	dest   = atm_state_create(fa);
	dbg_assert(source != NULL && dest != NULL);

	fa->start_state = source;
	ilist_add_back(fa->accept_states, dest);

	atm_transform_create(fa, source, first->start_state, Epsilon);
	atm_transform_create(fa, source, second->start_state, Epsilon);
	atm_transform_create(fa, (atm_state*)ilist_front(first->accept_states), dest, Epsilon);
	atm_transform_create(fa, (atm_state*)ilist_front(second->accept_states), dest, Epsilon);

	first->lifestate = atm_joined;
	second->lifestate = atm_joined;
	atm_destroy(first);
	atm_destroy(second);

	fa->lifestate = atm_active;
	return fa;
}

nfa* nfa_star  (nfa* fa) {
	atm_context* context = fa->context;
	atm_state *source, *dest;

	dbg_assert(ilist_size(fa->accept_states) == 1);

	source = atm_state_create(fa);
	dest   = atm_state_create(fa);
	dbg_assert(source != NULL && dest != NULL);

	fa->start_state = source;
	ilist_add_back(fa->accept_states, dest);

	atm_transform_create(fa, source, fa->start_state, Epsilon);
	atm_transform_create(fa, source, dest, Epsilon);
	atm_transform_create(fa, (atm_state*)ilist_front(fa->accept_states), dest, Epsilon);
	atm_transform_create(fa, (atm_state*)ilist_front(fa->accept_states), fa->start_state, Epsilon);

	fa->lifestate = atm_active;
	return fa;
}

nfa* nfa_plus  (nfa* n) {
	
}

nfa* nfa_range (nfa* n, int min_appear, int max_appear) {

}

nfa* nfa_from_string(const char* str, int length);
nfa* nfa_from_char  (char c);
