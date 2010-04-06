#include "regc.h"
#include "fa.h"
#include "heap2.h"

#include <stdio.h>

/* compiled regular expression */
#define REGC_BUFFER_SIZE 400
#define REG_RAWSTRING_SIZE 100
#define REG_TYPE_NONE 0
#define REG_TYPE_OP 1
#define REG_TYPE_FA 2

/* the strings below are use for printing */
const char *regc_str[] = {
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
			printf( " %s ", regc_str[-prc->buffer[i]] );
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

static int r_is_digit( char ch ) {
	return ch >= '0' && ch <= '9';
}

p_regc regc_compile( const char* str ) {
	p_regc prc;
	int size, num;
	int i, j;
	int last_j, is_prev_str;
	int last_type;

	size = 0;
	while ( *(str+size) != '\0' ) {
		size ++;
	}

	if ( size > REG_RAWSTRING_SIZE ) {
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
