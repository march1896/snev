#include "automaton.h"

#include <memheap/heap_global.h>
#include <cntr2/oallocator.h>
#include <cntr2/ifactory.h>
#include <cntr2/ilist.h>
#include <cntr2/iitr.h>
#include <cntr2/ialgo.h>


atm_context* atm_context_create() {
	return atm_context_create_v(__global_default_alloc, __global_default_dealloc, __global_default_heap, false);
}

atm_context* atm_context_create_v(pf_alloc alc, pf_dealloc dlc, void* heap, bool acc_cntr) {
	atm_context* context = NULL;
	dbg_assert(alc != NULL && dlc != NULL && heap != NULL);

	context = (atm_context*)alloc(alc, heap, sizeof(atm_context));
	context->next_id   = 1;
	context->__alloc   = alc;
	context->__dealloc = dlc;
	context->__heap    = heap;

	if (acc_cntr) {
		context->cntr_alc = allocator_mpool_spawn(__global_default_allocator, 20);
		context->join_cntr_alc = true;
	}
	else {
		context->cntr_alc = __global_default_allocator;
		context->join_cntr_alc = false;
	}

	context->atm_pool   = as_list(cntr_create_olist_v(context->cntr_alc));
	context->state_pool = as_list(cntr_create_olist_v(context->cntr_alc));
	context->trans_pool = as_list(cntr_create_olist_v(context->cntr_alc));


	return context;
}

void atm_context_destroy(atm_context* context) {
	ilist_destroy(context->atm_pool);
	ilist_destroy(context->state_pool);
	ilist_destroy(context->trans_pool);

	if (context->join_cntr_alc == true)
		allocator_join(context->cntr_alc);

	dealloc(context->__dealloc, context->__heap, context);
}

atm* atm_create(atm_context* context) {
	atm* ret;

	if (ilist_empty(context->atm_pool) == false) {
		atm* ret = (atm*)ilist_remove_back(context->atm_pool);
		dbg_assert(ret->lifestate == atm_invalid);
		dbg_assert(ret->context == context);
		dbg_assert(ret->start_state == NULL);
		dbg_assert(ret->accept_states != NULL && ilist_empty(ret->accept_states));
		dbg_assert(ret->states != NULL && ilist_empty(ret->states));

		return ret;
	} else {
		ret = (atm*)alloc(context->__alloc, context->__heap, sizeof(atm));

		ret->start_state   = NULL;
		ret->accept_states = as_list(cntr_create_olist_v(context->cntr_alc));
		ret->states        = as_list(cntr_create_olist_v(context->cntr_alc));

		ret->context   = context;
		ret->lifestate = atm_active; 

		return ret;
	}
	dbg_assert(false);

	return NULL;
}

void atm_destroy(atm* a) {
	atm_context* context = a->context;
	dbg_assert(a != NULL && context != NULL);

	if (a->lifestate == atm_invalid) {
		return;
	} else if (a->lifestate == atm_joined) {
		/* the states is joined to other automatons */
		ilist_clear(a->states);

		/* accept_states are just pointers */
		ilist_clear(a->accept_states);
	} else if (a->lifestate == atm_active) {
		foreach(ilist_itr_begin(a->states), ilist_itr_end(a->states), (pf_ref_visit)atm_state_destroy);
		ilist_clear(a->states);

		/* accept_states are just pointers */
		ilist_clear(a->accept_states);
	}

	a->lifestate = atm_invalid;

	ilist_add_back(a->context->atm_pool, (void*)a);
	return;
}

bool atm_check(atm_context* context, atm* a) {
	return true;
}

atm* atm_clone(atm* a) {

}

atm_trans* atm_transform_create(atm* a, atm_state* from, atm_state* to, unique_id trans_id) {
	atm_context* context = a->context;
	atm_trans* trans = NULL;
	dbg_assert(from != NULL && to != NULL && from != to);

	if (ilist_empty(context->trans_pool) == false) {
		trans = (atm_trans*)ilist_remove_back(context->trans_pool);
		dbg_assert(trans->from == NULL);
		dbg_assert(trans->to   == NULL);
	} else {
		trans = (atm_trans*)alloc(context->__alloc, context->__heap, sizeof(atm_trans));
	}

	dbg_assert(trans != NULL);

	trans->from = from;
	trans->to   = to;
	trans->id   = trans_id;

	ilist_add_back(from->transforms, trans);

	return trans;
}

void atm_transform_destroy(atm_trans* atm_trans) {
	atm_context* context = atm_trans->from->container->context;
	dbg_assert(context == atm_trans->to->container->context);

	atm_trans->from = NULL;
	atm_trans->to   = NULL;

	ilist_add_back(context->trans_pool, atm_trans);
}

atm_state* atm_state_create(atm* a) {
	atm_context* context = a->context;
	if (ilist_empty(context->state_pool) == false) {
		atm_state* pooled = (atm_state*)ilist_remove_back(context->state_pool);
		dbg_assert(pooled->container->context == context);
		dbg_assert(pooled->transforms != NULL && ilist_empty(pooled->transforms));

		return pooled;
	} else {
		atm_state* nstate = (atm_state*)alloc(context->__alloc, context->__heap, sizeof(atm_state));

		nstate->container = a;
		nstate->id        = atm_context_newid(context);
		nstate->priority  = 0;
		nstate->transforms = as_list(cntr_create_olist_v(context->cntr_alc));

		return nstate;
	}

	dbg_assert(false);

	return NULL;
}

void atm_state_destroy(atm_state* state) {
	foreach(ilist_itr_begin(state->transforms), ilist_itr_end(state->transforms), (pf_ref_visit)atm_transform_destroy);

	ilist_clear(state->transforms);

	ilist_add_back(state->container->context->state_pool, state);
}
