#include "fa.h"
#include "heap2.h"
#include "nfastack.h"
#include "reader.h"
#include "stdlib.h"

#include "stdio.h"

#define MEMORY_SIZE 1<<20

int main() {
	char* memory;
	memory = (char*)malloc( sizeof(char) * MEMORY_SIZE );
	InitHeap( memory, MEMORY_SIZE, 32 );

	//char* str = "a(b)|c[xyz]d*c{1,3}d(d|e|f)ada\\{\\}dfadf{3,5}x\\d*[a-z0-9XYZ][ABCD]\\[\\]";
	//char* str = "ab{1,4}";
	//char* str = "abc(d|e)?";
	char* patten = "https?://[0-9a-zX-Z_.]*";
	char* text = "http://www.google.com";
	p_dfa ppp = build_dfa_from_memory( patten );
	dfa_print( ppp );

	printf( "\n\npatten accept text: %s\n", dfa_accept_string( ppp, text ) ? "YES": "NO" );
	dfa_del( ppp );
		
	/*
	p_nfa pa = nfa_make_from_stringconcat( "" );
	nfa_print( pa );
	printf( "\n-------------------------\n" );

	p_nfa pb = nfa_make_from_stringconcat( "a" );
	nfa_print( pb );
	printf( "\n-------------------------\n" );

	p_nfa pc;
	pc = nfa_branch( pa, pb );
	nfa_del( pa );
	nfa_del( pb );

	nfa_print( pc );
	printf( "\n-------------------------\n" );

	pb = nfa_make_from_stringconcat( "aa" );
	nfa_print( pb );
	printf( "\n-------------------------\n" );

	p_nfa pd;
	pd = nfa_branch( pc, pb );
	nfa_del( pc );
	nfa_del( pb );

	nfa_print( pd );
	printf( "\n-------------------------\n" );

	p_dfa pda = dfa_convert_from_nfa( pd );

	dfa_print( pda );
	printf( "\n-------------------------\n" );

	nfa_del( pd );
	dfa_del( pda );
	*/
	CheckLeakPoint();

	DeinitHeap();
	free( memory );
	return 0;
}
