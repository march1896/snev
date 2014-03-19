#include "automaton.h"

#include <memheap/heap_global.h>
#include <cntr2/oallocator.h>
#include <cntr2/ifactory.h>
#include <cntr2/iobject.h>
#include <cntr2/ilist.h>
#include <cntr2/iitr.h>
#include <cntr2/ialgo.h>
#include <cntr2/iset.h>


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

	context->automatons = cntr_create_olist_a(pointer_traits, context->cntr_alc);

	return context;
}

void atm_context_destroy(atm_context* context) {
	foreach(ilist_itr_begin(context->automatons), ilist_itr_end(context->automatons), (pf_ref_visit)atm_destroy);

	if (context->join_cntr_alc == true)
		allocator_join(context->cntr_alc);

	dealloc(context->__dealloc, context->__heap, context);
}

atm* atm_create(atm_context* context) {
	atm* ret = (atm*)alloc(context->__alloc, context->__heap, sizeof(atm));

	ret->start_state   = NULL;
	ret->accept_states = cntr_create_olist_a(pointer_traits, context->cntr_alc);
	ret->states        = cntr_create_olist_a(pointer_traits, context->cntr_alc);

	ret->context       = context;
	ret->lifestate     = atm_active; 

	return ret;
}

void atm_destroy(atm* a) {
	atm_context* context = a->context;
	dbg_assert(a != NULL && context != NULL);

	dbg_assert(a->lifestate < atm_invalid);
	if (a->lifestate == atm_joined) {
		/* the states is joined to other automatons */
		iobject_destroy(a->states);
		/* accept_states are just pointers */
		iobject_destroy(a->accept_states);
	} else if (a->lifestate == atm_active) {
		/* first destroy each states which belongs to this automaton */
		foreach(ilist_itr_begin(a->states), ilist_itr_end(a->states), (pf_ref_visit)atm_state_destroy);
		iobject_destroy(a->states);
		/* accept_states are just pointers */
		iobject_destroy(a->accept_states);
	}

	a->lifestate = atm_invalid;

	dealloc(context->__dealloc, context->__heap, a);

	return;
}

bool atm_check(atm_context* context, atm* a) {
	return true;
}

int pair_pointer_compare(const void* ref_a, const void* ref_b) {
	struct pair* pa = (struct pair*)ref_a;
	struct pair* pb = (struct pair*)ref_b;

	if (pa->key < pb->key) 
		return -1;
	else if (pa->key > pb->key)
		return 1;

	return 0;
}

void pair_release(struct pair* p, void* context) {
	unused(context);
}

atm* atm_clone(atm* old_atm) {
	atm* new_atm = atm_create(old_atm->context);
	// TODO
	iset states_map = cntr_create_ollrb_a(pointer_traits, old_atm->context->cntr_alc);

	iterator itr = ilist_itr_create(old_atm->states, itr_begin);
	for (; itr != ilist_itr_end(old_atm->states); itr_to_next(itr)) {
		atm_state* state = (atm_state*)itr_get_obj(itr);
		atm_state* n_state = atm_state_create(new_atm);
		struct pair map_ele = {state, n_state};
		iset_insert(states_map, &map_ele);
	}
	itr_destroy(itr);
}

atm_state* atm_state_create(atm* a) {
	atm_context* context = a->context;
	atm_state* nstate = (atm_state*)alloc(context->__alloc, context->__heap, sizeof(atm_state));

	nstate->container = a;
	nstate->id        = atm_context_newid(context);
	nstate->priority  = 0;
	nstate->transforms = cntr_create_olist_a(pointer_traits, context->cntr_alc);

	ilist_add_back(a->states, nstate);

	return nstate;
}

void atm_state_destroy(atm_state* state) {
	foreach(ilist_itr_begin(state->transforms), ilist_itr_end(state->transforms), (pf_ref_visit)atm_transform_destroy);

	iobject_destroy(state->transforms);
}


void atm_transform_create(atm* a, atm_state* from, atm_state* to, unique_id trans_id) {
	atm_context* context = a->context;
	atm_trans* trans = (atm_trans*)alloc(context->__alloc, context->__heap, sizeof(atm_trans));;
	dbg_assert(from != NULL && to != NULL && from != to);
	dbg_assert(trans != NULL);

	trans->from = from;
	trans->to   = to;
	trans->id   = trans_id;

	ilist_add_back(from->transforms, trans);

	return;
}

void atm_transform_destroy(atm_trans* atm_trans) {
	atm_context* context = atm_trans->from->container->context;
	dbg_assert(context == atm_trans->to->container->context);

	dealloc(context->__dealloc, context->__heap, atm_trans);
}

