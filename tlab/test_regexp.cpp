#include "regexp.h"
#include "regexp_fa.h"
#include "stdio.h"
#include "stdlib.h"
#include "heap2.h"
using namespace regexp;

void teststring( const char* s, Context* con ) {
	printf( "%s\t%d\n", s, RegexpAcceptable( s , con ) );
	return;
}
int main() {
	const int INIT_MEMORY_SIZE = 1 << 16;
	void* memory = (void*)malloc( INIT_MEMORY_SIZE );
	if ( memory == NULL ) printf( "!!!!!FATAL ERROR!!!!!\n" );
	InitHeap( memory, INIT_MEMORY_SIZE );
	
	char buff[] = "(ab|a)*";
	Context* con;
	con = RegexpCompile( buff );
	OutputContext( con );
	//CheckLeakPoint();
	//printf( "After" );
	//CheckLeakPoint();
	CompileNfaToDfa( con );
	OutputDfa( con );
	printf( "hello, world\n" );

	teststring( "ababababababababababab", con );
	teststring( "abaabaaabaab", con );
	teststring( "abbaab", con );

	DestoryContext( con );
	CheckLeakPoint();

	//printf( "%d %d\n", NumNfaList( con ), NumNfaStack( con ) );

	free( memory );

	return 0;
}
