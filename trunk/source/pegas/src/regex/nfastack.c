#include "nfastack.h"
#include "fa.h"
#include "stdio.h"

p_stack stack_new() {
	p_stack ret;
	ret = (p_stack)t_alloc( sizeof( s_stack ) );
	ret->top = NULL;
	return ret;
}
void stack_del( p_stack ps ) {
	p_stack_ele prev, next;

	next = ps->top;
	while ( next != NULL ) {
		prev = next;
		next = next->next;
		t_free( prev );
	}

	t_free( ps );

	return;
}

void stack_push_op( p_stack ps, int op ) {
	p_stack_ele pse;

	pse = (p_stack_ele)t_alloc( sizeof( s_stack_ele ) );
	pse->type = e_op;
	pse->value.op = op;
	pse->next = ps->top;

	ps->top = pse;

	return;
}

void stack_push_nfa( p_stack ps, p_nfa pa ) {
	p_stack_ele pse;

	pse = (p_stack_ele)t_alloc( sizeof( s_stack_ele ) );
	pse->type = e_nfa;
	pse->value.pa = pa;
	pse->next = ps->top;

	ps->top = pse;
	return;
}

void stack_pop( p_stack ps ) {
	p_stack_ele pse;

	if ( ps->top != NULL ) {
		pse = ps->top;
		ps->top = ps->top->next;

		t_free( pse );
	}

	return;
}

p_stack_ele stack_top( p_stack ps ) {
	return ps->top;
}

void stack_process( p_stack ps, int op ) {
	p_stack_ele pse;
	p_nfa pa_x, pa_y, pa;
	int op_prev;

	if ( ps->top == NULL ) return;

	pse = stack_top( ps );
	if ( pse->type == e_op ) {
		printf( "FATAL: two continuous operator A\n" );
		return;
	}
	pa_x = pse->value.pa;
	stack_pop( ps );

	while ( ps->top != NULL ) {
		pse = stack_top( ps );
		if ( !pse ) {
			printf( "FATAL: empty stack\n" );
			return;
		}
		else if ( pse->type == e_nfa ) {
			printf( "FATAL: two continuous nfa\n" );
			return;
		}
		op_prev = pse->value.op;

		if ( op_prev == LEFT_PAR && op == RIGHT_PAR ) {
			stack_pop( ps );
			break;
		}
		if ( op_prev < op ) break;

		stack_pop( ps );

		pse = stack_top( ps );
		if ( !pse || pse->type != e_nfa ) {
			printf( "FATAL: two continuous operator\n" );
			return;
		}
		pa_y = pse->value.pa;
		stack_pop( ps );

		if ( op_prev == BRANCH ) {
			pa = nfa_branch( pa_y, pa_x );
			nfa_del( pa_y );
			nfa_del( pa_x );
		}
		else if ( op_prev == CONCAT ) {
			pa = nfa_concat( pa_y, pa_x );
			nfa_del( pa_y );
			nfa_del( pa_x );
		}

		pa_x = pa;

		if ( ps->top == NULL ) break;
	}

	stack_push_nfa( ps, pa_x );

	return;
}

