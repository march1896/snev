#ifndef _NFA_STACK_
#define _NFA_STACK_
#include "fa.h"

#define END 0
#define LEFT_PAR 3
#define RIGHT_PAR 4
#define BRANCH 5
#define CONCAT 6
typedef struct __s_stack_ele {
	enum {
		e_op,
		e_nfa,
	} type;
	union {
		int                             op;  // operator
		p_nfa                           pa;
	} value;
	struct __s_stack_ele*   next;
} s_stack_ele, *p_stack_ele;

typedef struct __s_stack {
	p_stack_ele                     top;
} s_stack, *p_stack;

p_stack stack_new();
void stack_del( p_stack ps );
void stack_push_op( p_stack ps, int op );
void stack_push_nfa( p_stack ps, p_nfa pa );
void stack_pop( p_stack ps );
p_stack_ele stack_top( p_stack ps );
void stack_process( p_stack ps, int op );

#endif //_NFA_STACK_
