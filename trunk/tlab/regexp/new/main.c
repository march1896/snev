#include "reg_def.h"
#include "heap2.h"
#include "stdlib.h"
#include "reader.h"

#include "stdio.h"

#define MEMORY_SIZE 1<<20

int main() {
	char* memory;
	memory = (char*)malloc( sizeof(char) * MEMORY_SIZE );
	InitHeap( memory, MEMORY_SIZE, 32 );

	//char* str = "abc*(d|ef)da\\{\\}dfadf{3,5}\\d{13,15}\\d*[^a-z0-9XYZ][ABCD]\\[\\]";
	char* str = "abc(d|e)?";
	p_dfa ppp = build_dfa_from_memory( str );
	dfa_print( ppp );
	dfa_del( ppp );
		
	/*
	p_nfa pa = nfa_make_from_stringconcat( str );
	nfa_print( pa );
	printf( "\n-------------------------\n" );

	p_nfa pb = nfa_make_from_stringbranch( str );
	nfa_print( pb );
	printf( "\n-------------------------\n" );

	p_nfa pc;
	pc = nfa_concat( pa, pb );
	nfa_del( pa );
	nfa_del( pb );

	nfa_print( pc );
	printf( "\n-------------------------\n" );

	p_nfa pd = nfa_closure( pc );
	nfa_del( pc );
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
