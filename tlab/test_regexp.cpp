#include "regexp.h"
#include "regexp_fa.h"
#include "stdio.h"
#include "stdlib.h"
#include "heap2.h"
using namespace regexp;

int main() {
	const int INIT_MEMORY_SIZE = 1 << 16;
	void* memory = (void*)malloc( INIT_MEMORY_SIZE );
	if ( memory == NULL ) printf( "!!!!!FATAL ERROR!!!!!\n" );
	InitHeap( memory, INIT_MEMORY_SIZE );
	
	char buff[] = "((ac))*|b";
	Context* con;
	con = RegexpCompile( buff );
	OutputContext( con );
	//CheckLeakPoint();
	//DestoryContext( con );
	//printf( "After" );
	//CheckLeakPoint();

	printf( "%d %d\n", NumNfaList( con ), NumNfaStack( con ) );

	free( memory );

	return 0;
}
