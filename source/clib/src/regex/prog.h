#ifndef _T_PROG_H_
#define _T_PROG_H_

/* I really don't know how to name this file, because the
 * functions here are something not relative enough.
 * but all of them are process used in regexp, so I name it'prog(rocess).h'
 * like generate nfa from regular expression,
 * check if the text is acceptable for a certain dfa,
 * print the dfa as the table format and etc.
 */
struct s_dfa;

p_dfa 	dfa_compile_from_string( const char* pattern );

int 	dfa_accept_string( const p_dfa pa, const char* text );

void 	dfa_find_in_text( const p_dfa pa, int greedy, int is_bol, const char* text, int* pos, int *len );

#endif // _T_PROG_H_
