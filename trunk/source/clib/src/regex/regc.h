#ifndef _REG_COMPILED_H_
#define _REG_COMPILED_H_
/* compiled regular expression module
 * to simplify the process of converting string to nfa,
 * I created a new format called 'compiled string', you 
 * can take it as a preprocesser of the regex
 */
#define REGC_BUFFER_SIZE 400

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
/* the macros are basic types used in regc */

typedef struct __s_regc {
	int* buffer;
	int size;
} s_regc, *p_regc;

p_regc regc_new();

void regc_del( p_regc prc );

p_regc regc_compile( const char* str );

void regc_print( p_regc prc );

#endif //_REG_COMPILED_H_
