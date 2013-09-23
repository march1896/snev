#ifndef _NONDETERMINSTIC_FINITE_AUTOMATON_H_
#define _NONDETERMINSTIC_FINITE_AUTOMATON_H_

#include <memheap/heap_def.h>
#include <cntr2/idef.h>

typedef struct automaton_transform {
	unique_id	            id;
	struct automaton_state* to;
	struct automaton_state* from; /* this is not needed if the transforms are saved by linked list */
} atm_trans;

typedef struct automaton_state {
	unique_id  id;
	int	       priority;
	ilist      transforms;       /* list of atm_trans; */

	struct automaton* container; /* the automaton which holds this state, this is not necessary */
} atm_state;

typedef struct automaton_context {
	pf_alloc    __alloc;         /* heap for the automatons, states and transforms */
	pf_dealloc  __dealloc;
	void*       __heap;

	ilist       atm_pool;      /* when the object of the context is destroyed, they are not dealloc but return to the pool */
	ilist       state_pool;
	ilist       trans_pool;

	allocator   cntr_alc;        /* container allocator */
	bool        join_cntr_alc;

	unique_id   next_id;
} atm_context;


typedef enum automaton_lifestate {
	atm_active,
	atm_joined,
	atm_invalid,
	atm_lifestate_count
} atm_lifestate;

typedef struct automaton {
	atm_lifestate lifestate;

	atm_state*   start_state;
	ilist        accept_states;  /* list of atm_state */
	ilist        states;         /* list of atm_state */

	atm_context* context;
} atm;

atm_context* atm_context_create();
atm_context* atm_context_create_v(pf_alloc alc, pf_dealloc dlc, void* heap, bool acc_cntr);
void         atm_context_destroy(atm_context* context);

unique_id    atm_context_newid(atm_context* context);

atm*         atm_create (atm_context* context);
void         atm_destroy(atm* a);
bool         atm_check  (atm_context* context, atm* a);
atm*         atm_clone  (atm* a);

atm_state*   atm_state_create(atm* a);
void         atm_state_destroy(atm_state* state);

/* after the creation, the transform is already added to the state's trans list */
atm_trans*   atm_transform_create(atm* a, atm_state* from, atm_state* to, unique_id trans_id);
void         atm_transform_destroy(atm_trans* atm_trans);

#endif /* _NONDETERMINSTIC_FINITE_AUTOMATON_H_ */
