#ifndef _NONDETERMINSTIC_FINITE_AUTOMATON_H_
#define _NONDETERMINSTIC_FINITE_AUTOMATON_H_

#include <memheap/heap_def.h>
#include <cntr2/idef.h>

typedef struct automaton nfa;
typedef struct automaton dfa;
typedef struct automaton_context fa_context;

fa_context* fa_context_create();
fa_context* fa_context_create_v(pf_alloc alc, pf_dealloc dlc, void* heap, bool acc_cntr);
void        fa_context_destroy(fa_context* context);

nfa* nfa_concat(nfa* first, nfa* second);
nfa* nfa_union (nfa* first, nfa* second);
nfa* nfa_star  (nfa* n);

nfa* nfa_plus  (nfa* n);
nfa* nfa_range (nfa* n, int min_appear, int max_appear);

nfa* nfa_from_string(const char* str, int length);
nfa* nfa_from_char  (char c);

#endif /* _NONDETERMINSTIC_FINITE_AUTOMATON_H_ */
