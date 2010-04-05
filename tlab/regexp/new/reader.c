#include "fa.h"
#include "nfastack.h"
#include "heap2.h"
#include "stdio.h"
#include "reader.h"

/*
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
*/

#define REG_ENDOFREG -1
#define REG_TAB -2
#define REG_NEWLINE -3
#define REG_BACKSPLAH -4
#define REG_BOL -5
#define REG_EOL -6
#define REG_NUMBER -7
#define REG_WORDS -8
#define REG_BLACK -9
#define REG_LEFTBRACE -10
#define REG_RIGHTBRACE -11
#define REG_LEFTPAR -12
#define REG_RIGHTPAR -13
#define REG_LEFTSQUARE -14
#define REG_RIGHTSQUARE -15
#define REG_STAR -16
#define REG_PLUS -17
#define REG_QUESTION -18
#define REG_WILDCAST -19
#define REG_BRANCH -20
#define REG_NOT -21
#define REG_NOT_WORDS -22
#define REG_NOT_NUMBER -23
#define REG_NOT_BLACK -24
#define REG_BETWEEN -25
#define REG_STRFRAGMENT -26
#define REG_CONCAT -27

#define REG_INFINITE 10000

const char *reg_str[] = {
	"ZERO",
	"ENDOFREG",
	"TAB",
	"NEWLINE",
	"BACKSPLAH",
	"BOL",
	"EOL",
	"NUMBER",
	"WORDS",
	"BLACK",
	"LEFTBRACE",
	"RIGHTBRACE",
	"LEFTPAR",
	"RIGHTPAR",
	"LEFTSQUARE",
	"RIGHTSQUARE",
	"STAR",
	"PLUS",
	"QUESTION",
	"WILDCAST",
	"BRANCH",
	"NOT",
	"NOT_WORDS",
	"NOT_NUMBER",
	"NOT_BLACK",
	"BETWEEN",
	"STRFRAGMENT",
 	"CONCAT",
};

/* compiled regular expression */
#define REGC_BUFFER_SIZE 400

p_regc regc_new() {
	p_regc prc;
	prc = (p_regc)t_alloc( sizeof( s_regc ) );
	prc->buffer = (int*)t_alloc( sizeof(int) * REGC_BUFFER_SIZE );
	prc->size = 0;
	return prc;
}

void regc_del( p_regc prc ) {
	t_free( prc->buffer );
	t_free( prc );
	return;
}

static int r_is_digit( char ch ) {
	return ch >= '0' && ch <= '9';
}

void regc_print( p_regc prc ) {
	int i; 
	int inbrace;
	
	if ( prc == NULL ) return;

	printf( "Compiled regexp size: %d\n", prc->size );
	inbrace = 0;
	for (i = 0; i < prc->size; i ++ ) {
		if ( prc->buffer[i] < 0 ) {
			if ( prc->buffer[i] == REG_LEFTBRACE ) inbrace = 1;
			else if ( prc->buffer[i] == REG_RIGHTBRACE ) inbrace = 0;
			printf( " %s ", reg_str[-prc->buffer[i]] );
		}
		else {
			if ( inbrace ) {
				printf( "::%d", prc->buffer[i] );
			}
			else {
				printf( "%c", (char)prc->buffer[i] );
			}
		}
	}

	printf( "END\n" );

	return;
}

#define REG_MAX_SIZE 100
#define REG_TYPE_NONE 0
#define REG_TYPE_OP 1
#define REG_TYPE_FA 2
p_regc regc_compile_from_memory( const char* str ) {
	p_regc prc;
	int size, num;
	int i, j;
	int last_j, is_prev_str;
	int last_type;

	size = 0;
	while ( *(str+size) != '\0' ) {
		size ++;
	}

	if ( size > REG_MAX_SIZE ) {
		printf( "error: reg extends the max size limit\n" );
		return NULL;
	}

	prc = regc_new();

	last_j = -1;
	is_prev_str = 0;
	last_type = REG_TYPE_NONE;
	for (i = 0, j = 0; i < size; ) {
		if ( str[i] == '{' ) {
			last_type = REG_TYPE_FA;
			if ( is_prev_str ) {
				if ( j == 0 ) {
					printf( "error: '{' has special meaning, should have a word before\n" );
					regc_del( prc );
					return NULL;
				}
				if ( prc->buffer[j-2] != REG_STRFRAGMENT ) {
					prc->buffer[j+1] = prc->buffer[j-1];
					prc->buffer[j-1] = REG_CONCAT;
					prc->buffer[j] = REG_STRFRAGMENT;
					j += 2;
				}
			}

			prc->buffer[j] = REG_LEFTBRACE;

			i ++;
			j ++;

			if ( str[i] == ',' ) {
				/* case {,... */
				prc->buffer[j] = 0;
				i ++;
				j ++;

				if ( str[i] == '}' ) {
					/* case {,} */
					prc->buffer[j++] = REG_INFINITE;
					prc->buffer[j++] = REG_RIGHTBRACE;
					i ++;
				}
				else if ( r_is_digit( str[i] ) ) {
					num = str[i++] - '0';
					while ( i < size && r_is_digit( str[i] ) ) {
						num *= 10;
						num += str[i++] - '0';
					}
					prc->buffer[j++] = num;

					if ( str[i] == '}' ) {
						prc->buffer[j++] = REG_RIGHTBRACE;
						i++;
					}
					else {
						/* error */
						printf( "error: unacceptable char in {}\n" );
						regc_del( prc );
						return NULL;
					}
				}
				else {
					/* error */
					printf( "error: unacceptable char in {}\n" );
					regc_del( prc );
					return NULL;
				}
			}
			else if ( r_is_digit( str[i] ) ) {
				/* case {num, ... */
				num = str[i++] - '0';
				while ( i < size && r_is_digit( str[i] ) ) {
					num *= 10;
					num += str[i++] - '0';
				}
				prc->buffer[j++] = num;

				if ( str[i] == '}' ) {
					/* case {num} */
					prc->buffer[j++] = num;
					prc->buffer[j++] = REG_RIGHTBRACE;

					i ++;
				}
				else if ( str[i] == ',' ) {
					i ++;
					if ( str[i] == '}' ) {
						/* case {num,} */
						prc->buffer[j++] = REG_INFINITE;
						prc->buffer[j++] = REG_RIGHTBRACE;

						i ++;
					}
					else if ( r_is_digit( str[i] ) ) {
						num = str[i++] - '0';
						while ( i < size && r_is_digit( str[i] ) ) {
							num *= 10;
							num += str[i++] - '0';
						}
						if ( num < prc->buffer[j] ) {
							printf( "error: the second number should be greater than first in {}\n" );
							regc_del( prc );
							return NULL;
						}
						else {
							prc->buffer[j++] = num;
						}

						if ( str[i] == '}' ) {
							prc->buffer[j++] = REG_RIGHTBRACE;
							i++;
						}
						else {
							printf( "error: unacceptable char in {}\n" );
							regc_del( prc );
							return NULL;
						}
					}
					else {
						printf( "error: unacceptable char in {}\n" );
						regc_del( prc );
						return NULL;
					}
				}
				else {
					printf( "error: unacceptable char in {}\n" );
					regc_del( prc );
					return NULL;
				}
			}
		}
		else if ( str[i] == '}' ) {
			printf( "error: } does not find the {\n" );
			regc_del( prc );
			return NULL;
		}
		else if ( str[i] == '[' ) {
			if ( last_type == REG_TYPE_FA ) {
				prc->buffer[j++] = REG_CONCAT;
			}
			last_type = REG_TYPE_FA;
			i ++;
			prc->buffer[j ++] = REG_LEFTSQUARE;
			if ( str[i] == '^' ) {
				i ++;
				prc->buffer[j++] = REG_NOT;
			}

			do { 
				if ( str[i] == ']' ) { break; }

				if ( str[i+1] == '-' ) {
					if ( str[i+2] < str[i] ) {
						printf( "error: char beside '-' in [], right should be greater then left\n" );
						regc_del( prc );
						return NULL;
					}
					prc->buffer[j++] = REG_BETWEEN;
					prc->buffer[j++] = (int)str[i];
					prc->buffer[j++] = (int)str[i+2];

					i += 3;
				}
				else {
					prc->buffer[j++] = (int)str[i++]; 
				}
			} while ( i < size );
			
			if ( str[i] == ']' ) {
				i ++;
				prc->buffer[j ++ ] = REG_RIGHTSQUARE;
			}
			else {
				/* error */
				printf( "error: [ does not find his ]\n" );
				regc_del( prc );
				return NULL;
			}
		}
		else if ( str[i] == ']' ) {
			printf( "error: ] does not find his [\n" );
			regc_del( prc );
			return NULL;
		}
		else if ( str[i] == '(' ) {
			if ( last_type == REG_TYPE_FA ) {
				prc->buffer[j++] = REG_CONCAT;
				last_type = REG_TYPE_OP;
			}
			prc->buffer[j++] = REG_LEFTPAR;
			i ++;
		}
		else if ( str[i] == ')' ) {
			last_type = REG_TYPE_FA;
			prc->buffer[j++] = REG_RIGHTPAR;
			i ++;
		}
		else if ( str[i] == '*' ) {
			last_type = REG_TYPE_FA;
			if ( is_prev_str ) {
				if ( j == 0 ) {
					printf( "error: '*' has special meaning, should have a word before\n" );
					regc_del( prc );
					return NULL;
				}
				if ( prc->buffer[j-2] != REG_STRFRAGMENT ) {
					prc->buffer[j+1] = prc->buffer[j-1];
					prc->buffer[j-1] = REG_CONCAT;
					prc->buffer[j] = REG_STRFRAGMENT;
					j += 2;
				}
			}
			prc->buffer[j++] = REG_STAR;
			i ++;
		}
		else if ( str[i] == '+' ) {
			last_type = REG_TYPE_FA;
			if ( is_prev_str ) {
				if ( j == 0 ) {
					printf( "error: '+' has special meaning, should have a word before\n" );
					regc_del( prc );
					return NULL;
				}
				if ( prc->buffer[j-2] != REG_STRFRAGMENT ) {
					prc->buffer[j+1] = prc->buffer[j-1];
					prc->buffer[j-1] = REG_CONCAT;
					prc->buffer[j] = REG_STRFRAGMENT;
					j += 2;
				}
			}
			prc->buffer[j++] = REG_PLUS;
			i ++;
		}
		else if ( str[i] == '?' ) {
			last_type = REG_TYPE_FA;
			if ( is_prev_str ) {
				if ( j == 0 ) {
					printf( "error: '?' has special meaning, should have a word before\n" );
					regc_del( prc );
					return NULL;
				}
				if ( prc->buffer[j-2] != REG_STRFRAGMENT ) {
					prc->buffer[j+1] = prc->buffer[j-1];
					prc->buffer[j-1] = REG_CONCAT;
					prc->buffer[j] = REG_STRFRAGMENT;
					j += 2;
				}
			}
			prc->buffer[j++] = REG_QUESTION;
			i ++;
		}
		else if ( str[i] == '|' ) {
			last_type = REG_TYPE_OP;
			prc->buffer[j++] = REG_BRANCH;
			i ++;
		}
		else if ( str[i] == '^' ) {
			if ( last_type == REG_TYPE_FA ) {
				prc->buffer[j++] = REG_CONCAT;
			}
			last_type = REG_TYPE_FA;
			prc->buffer[j++] = REG_BOL;
			i ++;
		}
		else if ( str[i] == '$' ) {
			if ( last_type == REG_TYPE_FA ) {
				prc->buffer[j++] = REG_CONCAT;
			}
			last_type = REG_TYPE_FA;
			prc->buffer[j++] = REG_EOL;
			i ++;
		}
		else if ( str[i] == '.' ) {
			if ( last_type == REG_TYPE_FA ) {
				prc->buffer[j++] = REG_CONCAT;
			}
			prc->buffer[j++] = REG_WILDCAST;
			i ++;
			is_prev_str = 1;
		}
		else if ( str[i] == '\\' ) {
			i++;
			if ( last_type == REG_TYPE_FA ) {
				if ( str[i] == 'd' || str[i] == 'D' || str[i] == 's' || str[i] == 'S' 
					|| str[i] == 'w' || str[i] == 'W' )
				prc->buffer[j++] = REG_CONCAT;
			}
			switch ( str[i] ) {
				case 'd':
					/* match only numbers */
					prc->buffer[j++] = REG_NUMBER;
					i ++;
					break;
				case 'D':
					/* match anything expect numbers */
					prc->buffer[j++] = REG_NOT_NUMBER;
					i ++;
					break;
				case 's':
					/* match black notation */
					prc->buffer[j++] = REG_BLACK;
					i ++;
					break;
				case 'S':
					/* match anything expect black notation */
					prc->buffer[j++] = REG_NOT_BLACK;
					i ++;
					break;
				case 'w':
					prc->buffer[j++] = REG_WORDS;
					i ++;
					/* match word */
					break;
				case 'W':
					prc->buffer[j++] = REG_NOT_WORDS;
					i ++;
					/* not a word */
					break;
				case '{':
				case '}':
				case '[':
				case ']':
				case '(':
				case ')':
				case '*':
				case '?':
				case '+':
				case '|':
				case '\\':
				case '^':
				case '$':
				case '.':
					/* match the charactor used for sepcial means */
					if ( is_prev_str ) {
						prc->buffer[j++] = (int)str[i++];
					}
					else {
						if ( last_type == REG_TYPE_FA ) {
							prc->buffer[j++] = REG_CONCAT;
						}
						prc->buffer[j++] = REG_STRFRAGMENT;
						prc->buffer[j++] = (int)str[i++];
						is_prev_str = 1;
					}
					last_type = REG_TYPE_FA;
					last_j = j;
					break;

				default:
					printf( "error: undefined escape character\n" );
					regc_del( prc );
					return NULL;
			}
		}
		else {
			if ( is_prev_str ) {
				prc->buffer[j++] = (int)str[i++];
			}
			else {
				if ( last_type == REG_TYPE_FA ) {
					prc->buffer[j++] = REG_CONCAT;
				}
				prc->buffer[j++] = REG_STRFRAGMENT;
				prc->buffer[j++] = (int)str[i++];
				is_prev_str = 1;
			}
			last_type = REG_TYPE_FA;

			last_j = j;
		}

		if ( last_j != j ) is_prev_str = 0;
	}

	prc->size = j;

	return prc;
}

p_regc regc_compile_from_rawstring( const char* str ) {
}

const char* chset_num = "0123456789";
const char* chset_word = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
const char* chset_black = " \t\f\r";

#define BUFF_SIZE 500
#define CHARACTER_SET_SIZE 128
p_dfa build_dfa_from_memory( char* str ) {
	p_dfa pdfa;
	p_regc prc;
	p_stack ps;
	p_stack_ele pse;
	p_nfa pa, pa_x, pa_y, pa_z, pa_w;
	char *pc0, *buffer, *pc1, *ch;
	int  v, i, j, k, x, y, z;

	prc = regc_compile_from_memory( str );

	if ( prc == NULL ) {
		return NULL;
	}

	regc_print( prc );

	ps = stack_new();
	buffer = (char*)t_alloc( sizeof(char) * BUFF_SIZE );
	ch = (char*)t_alloc( sizeof(char) * CHARACTER_SET_SIZE );

	/* TODO: check the str is comfortable */

	for (i = 0;i < prc->size; i++ ) {
		switch ( prc->buffer[i] ) {
			case REG_NUMBER:
				//printf( "%s\n", chset_num );
				pa = nfa_make_from_stringbranch( chset_num );
				stack_push_nfa( ps, pa );
				break;
			case REG_WORDS:
				//printf( "%s\n", chset_word );
				pa = nfa_make_from_stringbranch( chset_word );
				stack_push_nfa( ps, pa );
				break;
			case REG_BLACK:
				//printf( "%s\n", chset_black );
				pa = nfa_make_from_stringbranch( chset_black );
				stack_push_nfa( ps, pa );
				break;
			case REG_LEFTPAR:
				//stack_process( ps, LEFT_PAR );
				//stack_push_op( ps, CONCAT );
				stack_push_op( ps, LEFT_PAR );
				break;
			case REG_RIGHTPAR:
				stack_process( ps, RIGHT_PAR );
				/*
				if ( prc->buffer[i+1] == REG_STRFRAGMENT ) {
					stack_push_op( ps, CONCAT );
				}
				*/
				break;
			case REG_LEFTSQUARE:
				v = 1;
				if ( prc->buffer[++i] == REG_NOT ) {
					v = 0;
					i ++;
				}

				for (j = 1; j < CHARACTER_SET_SIZE; j ++ ) ch[j] = 0;
				do {
					if ( prc->buffer[i] == REG_RIGHTSQUARE ) break;
					else if ( prc->buffer[i] == REG_BETWEEN ) {
						i ++;
						x = prc->buffer[i++];
						y = prc->buffer[i++];
						if ( x > y ) {
							z = x;
							x = y;
							y = z;
						}
						while ( x <= y ) ch[x++] = 1;
					}
					else {
						x = prc->buffer[i++];
						ch[x] = 1;
					}
				} while ( i < prc->size );

				for (j = 1, k = 0; j < CHARACTER_SET_SIZE; j ++ ) {
					if ( !( ch[j] ^ v ) ) {
						buffer[k++] = (char)j;
					}
				}
				buffer[k] = '\0';
				//printf( "%s\n", buffer );
				pa = nfa_make_from_stringbranch( buffer );
				stack_push_nfa( ps, pa );
				break;
			case REG_RIGHTSQUARE:
				printf( "FATALERROR: right square\n" );
				break;
			case REG_LEFTBRACE:
				pse = stack_top( ps );

				if ( pse->type != e_nfa ) {
					printf( "error!" );
					break;
				}

				i ++;
				x = prc->buffer[i++];
				y = prc->buffer[i++];

				pa = pse->value.pa;

				pa_z = nfa_multiple( pa, x );

				if ( y == REG_INFINITE ) {
					pa_y = nfa_closure( pa );
				}
				else {
					for (j = 0; j <= y-x; j ++ ) {
						pa_x = nfa_multiple( pa, j );

						if ( j == 0 ) {
							pa_y = nfa_copy( pa_x );
						}
						else {
							pa_w = pa_y;
							pa_y = nfa_branch( pa_w, pa_x );
							nfa_del( pa_w );
						}
						nfa_del( pa_x );
					}
				}

				nfa_del( pa );
				pa = nfa_concat( pa_z, pa_y );
				nfa_del( pa_z );
				nfa_del( pa_y );
				
				stack_pop( ps );
				stack_push_nfa( ps, pa );
				break;
			case REG_RIGHTBRACE:
				printf( "FATALERROR: right brace\n" );
				break;
			case REG_BRANCH:
				stack_process( ps, BRANCH );
				stack_push_op( ps, BRANCH );
				break;
			case REG_CONCAT:
				stack_process( ps, CONCAT );
				stack_push_op( ps, CONCAT );
				break;
			case REG_STAR:
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
			case REG_PLUS:
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
			case REG_QUESTION:
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
			case REG_WILDCAST:
				for (j = 1, k = 0; j < CHARACTER_SET_SIZE; j ++ ) {
					if ( j != '\n' ) {
						buffer[k++] = (char)j;
					}
				}
				buffer[k] = '\0';
				//printf( "%s\n", buffer );
				pa = nfa_make_from_stringbranch( buffer );
				stack_push_nfa( ps, pa );
				break;
			case REG_BOL:
				pa = nfa_make_from_stringconcat( "\n" );
				stack_push_nfa( ps, pa );
				break;
			case REG_EOL:
				pa = nfa_make_from_stringconcat( "\n" );
				stack_push_nfa( ps, pa );
				break;
			case REG_STRFRAGMENT:
				i ++;
				for (j = i; j < prc->size; j ++ ) {
					if ( prc->buffer[j] <= 0 ) {
						buffer[j-i] = '\0';
						break;
					}
					else {
						buffer[j-i] = (char)prc->buffer[j];
					}
				}
				if ( j == prc->size ) buffer[j-i] = '\0';

				//printf( "%d %s\n", j, buffer );
				pa = nfa_make_from_stringconcat( buffer );
				stack_push_nfa( ps, pa );

				i = j-1;
				break;
			default:
				printf( "FATAL ERROR %s\n", reg_str[ -prc->buffer[i] ] );
		}
	}

	stack_process( ps, END );

	if ( ps->top->next != NULL ) printf( "error!\n" );
	pse = stack_top( ps );
	pa = pse->value.pa;

	regc_del( prc );

	pdfa = dfa_convert_from_nfa( pa );
	nfa_del( pa );

	stack_del( ps );
	t_free( ch );
	t_free( buffer );
	return pdfa;
}
