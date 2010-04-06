#include "fa.h"
#include "nfastack.h"
#include "regc.h"
#include "heap2.h"

#include <stdio.h>

const char* chset_num = "0123456789";
const char* chset_word = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
const char* chset_black = " \t\f\r";

#define BUFF_SIZE 256
#define CHARACTER_SET_START 32
#define CHARACTER_SET_END 128
extern const char* regc_str[];
p_dfa dfa_compile_from_string( const char* str ) {
	p_dfa pdfa;
	p_regc prc;
	p_stack ps;
	p_stack_ele pse;
	p_nfa pa, pa_x, pa_y, pa_z, pa_w;
	char *pc0, *buffer, *pc1, *ch;
	int  v, i, j, k, x, y, z;

	prc = regc_compile( str );

	if ( prc == NULL ) {
		return NULL;
	}

	regc_print( prc );

	ps = stack_new();
	buffer = (char*)t_alloc( sizeof(char) * BUFF_SIZE );
	ch = (char*)t_alloc( sizeof(char) * CHARACTER_SET_END );

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
				stack_push_op( ps, LEFT_PAR );
				break;
			case REG_RIGHTPAR:
				stack_process( ps, RIGHT_PAR );
				break;
			case REG_LEFTSQUARE:
				v = 1;
				if ( prc->buffer[++i] == REG_NOT ) {
					v = 0;
					i ++;
				}

				for (j = 0; j < CHARACTER_SET_END; j ++ ) ch[j] = 0;
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

				for (j = CHARACTER_SET_START, k = 0; j < CHARACTER_SET_END; j ++ ) {
					if ( !( ch[j] ^ v ) ) {
						buffer[k++] = (char)j;
					}
				}
				if ( !( ch['\t'] ^ v ) ) { buffer[k++] = (char)'\t'; }
				if ( !( ch['\n'] ^ v ) ) { buffer[k++] = (char)'\n'; }
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
				for (j = CHARACTER_SET_START, k = 0; j < CHARACTER_SET_END; j ++ ) {
					buffer[k++] = (char)j;
				}
				buffer[k++] = '\t';
				buffer[k] = '\0';
				//printf( "%s\n", buffer );
				pa = nfa_make_from_stringbranch( buffer );
				stack_push_nfa( ps, pa );
				break;
			case REG_BOL:
				pa = nfa_make_from_stringbranch( "\2\n" );
				stack_push_nfa( ps, pa );
				break;
			case REG_EOL:
				pa = nfa_make_from_stringbranch( "\3\n" );
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
				printf( "FATAL ERROR %s\n", regc_str[ -prc->buffer[i] ] );
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

int dfa_accept_string( const p_dfa dfa, const char* text ) {
	p_nodelist pnl;
	p_node pn;
	p_edgelist pel;
	p_edge pe;
	char str[BUFF_SIZE+3];
	int len, i;

	if ( dfa == NULL ) return 0;

	pnl = dfa->pnl_f;
	while ( pnl != NULL ) {
		pn = pnl->element;

		if ( pn->info & NODE_START ) {
			break;
		}
		pnl = pnl->next;
	}

	if ( pnl == NULL ) return 0;

	/* push front and push back '\n' to enable match ^ $ */
	len = strlen( text );
	//str = (char*)t_alloc( sizeof( char )*(len + 10) );
	str[0] = '\2';
	for (i = 0; i < len; i ++ ) {
		str[i+1] = text[i];
	}
	str[i+1] = '\3';
	str[i+2] = '\0';
	len = i + 2;

	printf( "after emendation for ^ and $: %s\n", str );

	for (i = 0; i < len; i ++) {
		pel = pn->pel_f;
		while ( pel != NULL ) {
			pe = pel->element;
			if ( pe->weight == (int)(str[i]) ) {
				pn = pe->dest;
				break;
			}
			pel = pel->next;
		}
		if ( pel == NULL ) {
			/* edge not found */
			if ( str[i] != '\2' && str[i] != '\3' ) {
				return 0;
			}
		}
	}

	//t_free( str );

	return pn->info & NODE_ACCEPT;
}

void dfa_find_in_text( const p_dfa dfa, int greedy, int is_bol, const char* text, int* pos, int *length ) {
	p_nodelist pnl;
	p_node pn;
	p_edgelist pel;
	p_edge pe;
	int len, i, j, found;
	int bol, eol;

	if ( dfa == NULL ) return; 

	pnl = dfa->pnl_f;
	while ( pnl != NULL ) {
		pn = pnl->element;

		if ( pn->info & NODE_START ) {
			break;
		}
		pnl = pnl->next;
	}

	if ( pnl == NULL ) return;

	/* push front and push back '\n' to enable match ^ $ */
	len = strlen( text );

	//printf( "after emendation for ^ and $: %s\n", text );

	found = 0;
	for (j = 0; j < len; j ++ ) {
		for (i = j; i < len; i ++) {
			if ( pn->info & NODE_ACCEPT ) {
				/* not greedy, if we found, return */
				found = 1;
				*pos = j;
				*length = i;
				if ( !greedy ) break;
			}

			pel = pn->pel_f;
			bol = eol = 0;
			while ( pel != NULL ) {
				pe = pel->element;
				if ( pe->weight == '\2' ) bol = 1;
				if ( pe->weight == '\3' ) eol = 1;
				if ( pe->weight == (int)(text[i]) ) {
					pn = pe->dest;
					break;
				}
				pel = pel->next;
			}

			if ( pel == NULL ) {
				/* edge not found */
				if ( is_bol && i == 0 && bol || i == len - 1 && eol ) {
				}
				else {
					break;
				}
			}
		}
		if ( found ) break;
	}

	if ( !found ) *pos = -1, *length = -1;
	return ;
}
