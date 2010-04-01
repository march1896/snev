#include "reg_def.h"

typedef struct __s_nfa_stack_element {
	p_nfa 	nfa;
	struct __s_nfa_stack_element* next;
} s_nfa_stack_element, *p_nfa_stack_element;

typedef struct __s_nfa_stack {
	p_nfa_stack_element 	top;
} s_nfa_stack, *p_nfa_stack;

p_nfa_stack nfa_stack_new() {
}

void nfa_stack_del( p_nfa_stack pnas ) {
}

void nfa_stack_push( p_nfa_stack pnas, p_nfa pa ) {
}

p_nfa nfa_stack_pop( p_nfa_stack pnas ) {
}

p_nfa nfa_stack_top( p_nfa_stack pnas ) {
}


p_dfa build_dfa_from_string( const char* str ) {

}
