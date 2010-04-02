#include "reg_def.h"
#include "heap2.h"

/*
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
*/
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
		int 				op;  // operator
		p_nfa 				pa;
	} value;
	struct __s_stack_ele* 	next;
} s_stack_ele, *p_stack_ele;

typedef struct __s_stack {
	p_stack_ele 			top;
} s_stack, *p_stack;

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

const char* b_t = "\t";
const char* b_n = "\n";
const char* b_r = "\r";
const char* b_bs= "\\";
const char* b_bn = "^";
const char* b_ed = "$";
const char* b_num = "0123456789";
const char* b_word = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
const char* b_black = " \t\f";

p_nfa nfa_get_from_specialchar( char c ) {
	p_nfa pa;
	const char* str;

	switch ( c ) {
		case 't':
			str = b_t;
			break;
		case 'n':
			str = b_n;
			break;
		case 'r':
			str = b_r;
			break;
		case '\\':
			str = b_bs;
			break;
		case '^':
			str = b_bn;
			break;
		case '$':
			str = b_ed;
			break;
		case 'd':
			str = b_num;
			break;
		case 'w':
			str = b_word;
			break;
		case 's':
			str = b_black;
			break;
	}
	
	pa = nfa_make_from_stringbranch( str );

	return pa;
}

void stack_process( p_stack ps, int op ) {
	p_stack_ele pse;
	p_nfa pa_x, pa_y, pa;
	int op_prev;

	if ( ps->top == NULL ) return;

	pse = stack_top( ps );
	if ( pse->type == e_op ) {
		printf( "%c: two continuous operator A\n" );
		return;
	}
	pa_x = pse->value.pa;
	stack_pop( ps );

	while ( ps->top != NULL ) {
		pse = stack_top( ps );
		if ( !pse ) {
			printf( "%c: empty stack\n" );
			return;
		}
		else if ( pse->type == e_nfa ) {
			printf( "%c: two continuous nfa\n" );
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
			printf( "%c: two continuous operator\n" );
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

int is_normal_char( char c ) {
	//printf( "is normal %c\n", c );
	if ( c == '*' || c == '+' || c == '?' || c == '|' ||
			c == '\\' || c == '{' || c == '}' ||
			c == '[' || c == ']' || c == '(' || c == ')' ||
			c == 0  )
		return 0;
	return 1;
}

#define MAX_RIGHT_BOUND 20
void find_bound( const char* str, int *lb, int* rb ) {
	int t;

	str ++;
	if ( *str == ',' ) *lb = 0;

}

#define BUFF_SIZE 100
p_dfa build_dfa_from_string( char* str ) {
	p_dfa pdfa;
	p_stack ps;
	p_stack_ele pse;
	p_nfa pa, pa_x, pa_y;
	char *pc0, *buffer, *pc1;
	int  v;

	ps = stack_new();
	buffer = (char*)t_alloc( sizeof(char) * BUFF_SIZE );

	/* TODO: check the str is comfortable */

	while ( *str != '\0' ) {
		switch ( *str ) {
			case '\\':
				str ++;
				pa = nfa_get_from_specialchar( *str );
				stack_push_nfa( ps, pa );
				break;
			case '(':
				//stack_process( ps, LEFT_PAR );
				stack_push_op( ps, CONCAT );
				stack_push_op( ps, LEFT_PAR );
				break;
			case ')':
				stack_process( ps, RIGHT_PAR );
				break;
			case '[':
				break;
			case ']':
				break;
			case '{':
				find_bound( str, &left_b, &right_b );
				break;
			case '}':
				break;
			case '|':
				stack_process( ps, BRANCH );
				stack_push_op( ps, BRANCH );
				break;
			case '*':
				pse = stack_top( ps );

				if ( pse->type != e_nfa ) {
					printf( "error!" );
					break;
				}
				pa = pse->value.pa;
				pa_x = nfa_closure( pa );
				nfa_del( pa );

				stack_pop( ps );
				stack_push_nfa( ps, pa_x );
				break;
			case '+':
				pse = stack_top( ps );

				if ( pse->type != e_nfa ) {
					printf( "error!" );
					break;
				}
				pa = pse->value.pa;
				pa_x = nfa_closure( pa );
				pa_y = nfa_copy( pa );
				nfa_del( pa );

				pa = nfa_concat( pa_y, pa_x );
				nfa_del( pa_x );
				nfa_del( pa_y );

				stack_pop( ps );
				stack_push_nfa( ps, pa );
				break;
			case '?':
				pse = stack_top( ps );

				if ( pse->type != e_nfa ) {
					printf( "error!" );
					break;
				}
				pa = pse->value.pa;
				pa_x = nfa_make_from_stringconcat( "" );

				pa_y = nfa_branch( pa, pa_x );
				nfa_del( pa_x );
				nfa_del( pa );

				stack_pop( ps );
				stack_push_nfa( ps, pa_y );
				break;
			case '.':
				break;
			case '^':
				break;
			case '$':
				break;
			default:
				/* normal char */
				pc0 = str;
				pc1 = buffer;

				while ( is_normal_char( *pc0 ) ) {
					*pc1 ++ = *pc0 ++;
				}

				if ( *pc0 != '*' && *pc0 != '{' && *pc0 != '+' && *pc0 != '?' ) {
					*pc1 = '\0';
					printf( "%s\n", buffer );
					pa = nfa_make_from_stringconcat( buffer );

					/*
					stack_process( ps, CONCAT );
					if ( ps->top != NULL ) {
						stack_push_op( ps, CONCAT );
					}
					*/
					stack_push_nfa( ps, pa );

					str = pc0-1;
				}
				else {
					pc0 --;
					pc1 --;

					if ( pc0 != str ) {
						/* two or more continues char */
						*pc1 = '\0';
						pa = nfa_make_from_stringconcat( buffer );

						//stack_push_op( ps, CONCAT );
						stack_push_nfa( ps, pa );

						//stack_process( ps, CONCAT );
						stack_push_op( ps, CONCAT );
					}

					*buffer = *pc0;
					*(buffer+1) = '\0';
					printf( "buffer %s\n", buffer );

					pa = nfa_make_from_stringconcat( buffer );
					stack_push_nfa( ps, pa );

					str = pc0;
				}
				break;
		}
		str ++;
	}

	stack_process( ps, END );

	if ( ps->top->next != NULL ) printf( "error!\n" );
	pse = stack_top( ps );
	pa = pse->value.pa;

	pdfa = dfa_convert_from_nfa( pa );
	nfa_del( pa );

	stack_del( ps );
	t_free( buffer );
	return pdfa;
}
