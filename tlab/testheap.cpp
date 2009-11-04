#include "heap.h"
#include "heap2.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

static uint Mylog2( uint x ) {
	// assert( x > 0 );
	int ret = 31;
	while ( ( x & ( 0x00000001 << ret ) ) == 0 ) {
		printf ("ret %d\n", ret );
		ret --;
	}
	return ret;
}

int main() {
	int stopflag;

	void* memory = (void*)malloc( 2 << 20 );
	InitHeap( memory, 2<<20 );

	printf( "0x%08x\n", 1 << 31 );
	printf( "0x%08x\n", 2 << 20 );
	printf( "%d\n", Mylog2( 2 << 20 ) );

	DumpFreeList();

	const int SIZE = 100;
	char* pointer[ SIZE ];
	memset( pointer, 0, sizeof( char* )*SIZE );

	for ( int i = 0; i < 100; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % 300 + 1;
			printf( "Allocate: %d\t\t", size );
			
			pointer[ num ] = (char*)Allocate( size * sizeof( char ) );
			if ( pointer[ num ] != NULL ) {
				printf( "Pointer %d \tallocate %d, \taddr 0x%08x\n", num, size, (unsigned)pointer[ num ] );
			}
		}
		else {
			printf( "Free:\t\t" );
			Free( pointer[num] );
			printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", num, GetMemoryBlockSize( pointer[ num] ), (unsigned)pointer[ num ] - 16 );
			pointer[ num ] = 0;
		}
		DumpFreeList();
		//heap->OutputFreeList();
		printf( "\n" );
	}


	free( memory );

	/*
	CHeap1* heap = new CHeap1();
	void *memory = (void*)malloc( 2 << 20 );
	heap->CreateFromBuffer( memory, 2 << 20 );

	heap->OutputFreeList();
	printf( "\n" );

	const int SIZE = 1000;
	char* pointer[ SIZE ];
	memset( pointer, 0, sizeof( char* )*SIZE );

	for ( int i = 0; i < 10000; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % 500;
			
			pointer[ num ] = (char*)heap->Alloc( size * sizeof( char ) );
			if ( pointer[ num ] != NULL ) {
				printf( "Pointer %d \tallocate %d, \taddr 0x%08x\n", num, size, (unsigned)pointer[ num ] );
			}
		}
		else {
			heap->Free( pointer[num] );
			pointer[ num ] = 0;
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", num, heap->GetBlockSize( pointer[ num] ), (unsigned)pointer[ num ] - 24 );
		}
		heap->OutputFreeList();
		printf( "\n" );
	}

	printf( "End\n\n\n" );
	//heap->OutputFreeList();

	for ( int i = 0; i < SIZE; i ++ ) {
		if ( pointer[ i ] ) {
			heap->Free( pointer[ i ] );
			printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", i, heap->GetBlockSize( pointer[ i ] ), (unsigned)pointer[ i ] - 24 );
			heap->OutputFreeList();
			printf( "\n" );
		}
	}
	heap->OutputFreeList();
	
	delete heap;
	*/
	//scanf("%d", &stopflag );
	return 0;
}
