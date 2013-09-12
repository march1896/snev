#include "automaton.h"

#include <memheap/heap_global.h>
#include <cntr2/oallocator.h>

typedef int charset;

typedef struct automaton_transform {
	charset	                id;
	struct automaton_state* dest;
} atm_transform;

typedef struct automaton_state {
	unique_id  id;
	int	       priority;
	ilist      transforms;     /* list of atm_trans; */
} atm_state;

typedef struct automaton_context {
	pf_alloc    __alloc;
	pf_dealloc  __dealloc;
	void*       __heap;

	allocator   cntr_alc;      /* container allocator */

	unique_id   next_id;
} atm_context;

typedef struct automaton {
	atm_state*   start_state;
	ilist        accept_states;  /* list of atm_state */
	ilist        states;         /* list of atm_state */

	atm_context* context;
} atm;

fa_context* fa_context_create() {
	fa_context* context = NULL;
	dbg_assert(alc != NULL && dlc != NULL && heap != NULL);

	context = (atm_context*)malloc(sizeof(atm_context));
	dbg_assert(context != NULL);
	context->next_id   = 1;
	context->__alloc   = __global_default_alloc;
	context->__dealloc = __global_default_dealloc;
	context->__heap    = __global_default_heap;

	context->cntr_alc  = NULL;

	return context;
}

atm_context* fa_context_create_v(pf_alloc alc, pf_dealloc dlc, void* heap, bool acc_cntr) {
	atm_context* context = NULL;
	dbg_assert(alc != NULL && dlc != NULL && heap != NULL);

	context = (atm_context*)malloc(sizeof(atm_context));
	context->next_id   = 1;
	context->__alloc   = alc;
	context->__dealloc = dlc;
	context->__heap    = heap;

	if (acc_cntr)
		context->cntr_alc = allocator_mpool_spawn(__global_default_allocator, 20);
	else 
		context->cntr_alc = NULL;

	return context;
}

void fa_context_destroy(atm_context* context) {
	if (context->cntr_alc != NULL)
		allocator_join(context->cntr_alc);

	free(context);
}

static nfa* nfa_create(atm_context* context) {
	nfa* n = (nfa*)alloc(context->__alloc, context->__heap, sizeof(nfa));

	n->context = context;
}

nfa* nfa_concat(nfa* first, nfa* second) {
	nfa* combined = NULL;

	dbg_assert(first != NULL && second != NULL);
	dbg_assert(first->context == second->context);

	return NULL;
}

nfa* nfa_union (nfa* first, nfa* second) {
	return NULL;
}

nfa* nfa_star  (nfa* n) {
	return NULL;
}

nfa* nfa_plus  (nfa* n);
nfa* nfa_range (nfa* n, int min_appear, int max_appear);

nfa* nfa_from_string(const char* str, int length);
nfa* nfa_from_char  (char c);