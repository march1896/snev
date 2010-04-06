#include "fa.h"
#include "heap2.h"
#include "nfastack.h"
#include "prog.h"
#include "stdlib.h"

#include "stdio.h"

#define MEMORY_SIZE 1<<20

int main() {
	char* memory;
	memory = (char*)malloc( sizeof(char) * MEMORY_SIZE );
	InitHeap( memory, MEMORY_SIZE, 32 );

	int c;
	for (c = 0; c < 32; c ++ ) {
		printf( "%d: %c\n", c, (char)c );
	}

	char* pattern = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
	char* text = "202.114.5.127";
	p_dfa ppp = dfa_compile_from_string( pattern );

	printf( "patten accept text: %s\n", dfa_accept_string( ppp, text ) ? "YES": "NO" );
	dfa_print( ppp );
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
