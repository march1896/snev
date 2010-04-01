#include "reg_def.h"
#include "heap2.h"
#include "stdlib.h"

#include "stdio.h"

#define MEMORY_SIZE 1<<20
int main() {
	char* memory;
	memory = (char*)malloc( sizeof(char) * MEMORY_SIZE );
	InitHeap( memory, MEMORY_SIZE, 32 );

	const char* str = "Tangel";
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

	p_nfa pd = nfa_closure( pc );
	nfa_del( pc );
	nfa_print( pd );
	printf( "\n-------------------------\n" );

	p_dfa pda = dfa_convert_from_nfa( pd );

	dfa_del( pda );
	nfa_del( pd );
	CheckLeakPoint();

	DeinitHeap();
	free( memory );
	return 0;
}
