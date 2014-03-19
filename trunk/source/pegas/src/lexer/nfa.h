#ifndef _LEXER_NONDETERMINISTIC_STATE_MACHINE_H_
#define _LEXER_NONDETERMINISTIC_STATE_MACHINE_H_

#include <util/cominc.h>

typedef struct automaton nfa;

#define Epsilon 256

nfa* nfa_concat(nfa* first, nfa* second);
nfa* nfa_union (nfa* first, nfa* second);
nfa* nfa_star  (nfa* n);

nfa* nfa_plus  (nfa* n);
nfa* nfa_range (nfa* n, int min_appear, int max_appear);

nfa* nfa_from_string(const char* str, int length);
nfa* nfa_from_char  (char c);

#endif /* _LEXER_NONDETERMINISTIC_STATE_MACHINE_H_ */
