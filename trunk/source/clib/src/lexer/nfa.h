#ifndef _NONDETERMINSTIC_FINITE_AUTOMATON_H_
#define _NONDETERMINSTIC_FINITE_AUTOMATON_H_

typedef int unique_id;
typedef int charset;

struct nfa_transform {
	charset           weight;

	struct nfa_state* dest;

	/* link to make self in a list */
	struct list_link  link;
};

struct nfa_state {
	unique_id         id;

	struct list_link  transforms;

	/* link to make self in a list */
	struct list_link  link;
};

struct nfa {
	struct nfa_state* start;
	struct nfa_state* accept;

	struct list_link  states;

	/* link to make self in a list */
	struct list_link  link;
};

struct nfa_transform* nfa_transform_new(charset weight, 

struct nfa* nfa_concat(struct nfa* first, struct nfa* second);
struct nfa* nfa_union (struct nfa* first, struct nfa* second);
struct nfa* nfa_star  (struct nfa* n);

struct nfa* nfa_plus  (struct nfa* n);
struct nfa* nfa_range (struct nfa* n, int min_appear, int max_appear);

struct nfa* nfa_from_string  (const char* str);
struct nfa* nfa_from_string_l(const char* str, int length);
struct nfa* nfa_from_char    (char c);

struct fa_context {
	pf_alloc    __alloc;
	pf_dealloc  __dealloc;
	void*       __heap;

	unique_id   next_id;
};

extern struct fa_context fa_context;

struct static_fa_context fa_context = {
	__global_default_alloc, 
	__global_default_dealloc,
	__global_default_heap,

	1
};

#endif /* _NONDETERMINSTIC_FINITE_AUTOMATON_H_ */
