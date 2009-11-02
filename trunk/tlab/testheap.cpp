#include "heap.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

int main() {
	int stopflag;
	CHeap1* heap = new CHeap1();
	void *memory = (void*)malloc( 2 << 20 );
	heap->CreateFromBuffer( memory, 2 << 20 );

	/*
	int *mint = (int*)heap->Alloc( sizeof( int ) * 10 );
	if ( mint == NULL ) printf( "mint alloc failed\n" );
	float *mfloat = (float*)heap->Alloc( sizeof( float ) * 40 );
	if ( mfloat == NULL ) printf( "mfloat alloc failed\n" );
	short *mshort = (short*)heap->Alloc( sizeof( short ) * 13 );
	if ( mshort == NULL ) printf( "mshort alloc failed\n" );

	printf( "mint addr: 0x%08x\n", (unsigned)mint );
	printf( "mfloat addr: 0x%08x\n", (unsigned)mfloat );
	printf( "mshort addr: 0x%08x\n", (unsigned)mshort );
	
	heap->Free( mfloat );

	char *mchar = (char*)heap->Alloc( sizeof( char ) * 15 );
	char *mchar2 = (char*)heap->Alloc( sizeof( char ) * 7 );
	char *mchar3 = (char*)heap->Alloc( sizeof( char ) * 3 );
	char *mchar4 = (char*)heap->Alloc( sizeof( char ) * 100 );
	printf( "mint addr: 0x%08x\n", (unsigned)mint );
	printf( "mchar addr: 0x%08x\n", (unsigned)mchar );
	printf( "mchar2 addr: 0x%08x\n", (unsigned)mchar2 );
	printf( "mchar3 addr: 0x%08x\n", (unsigned)mchar3 );
	printf( "mchar4 addr: 0x%08x\n", (unsigned)mchar4 );
	printf( "mshort addr: 0x%08x\n", (unsigned)mshort );
	*/

	heap->OutputFreeList();
	printf( "\n" );

	char* pointer[ 100 ];
	int allocated[ 100 ];
	memset( allocated, 0, sizeof( int )*100 );

	for ( int i = 0; i < 100; i ++ ) {
		int num = rand() % 100;
		if ( allocated[ num ] == 0 ) {
			int size = rand() % 200;
			
			pointer[ num ] = (char*)heap->Alloc( size * sizeof( char ) );
			if ( pointer[ num ] != NULL ) {
				printf( "Pointer %d \tallocate %d, \taddr 0x%08x\n", num, size, (unsigned)pointer[ num ] );
				allocated[ num ] = 1;
			}
		}
		else {
			heap->Free( pointer[num] );
			allocated[ num ] = 0;
			printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", num, heap->GetBlockSize( pointer[ num] ), (unsigned)pointer[ num ] - 24 );
		}
		heap->OutputFreeList();
		printf( "\n" );
	}

	printf( "End\n\n\n" );
	//heap->OutputFreeList();

	for ( int i = 0; i < 100; i ++ ) {
		if ( allocated[ i ] ) {
			heap->Free( pointer[ i ] );
			printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", i, heap->GetBlockSize( pointer[ i ] ), (unsigned)pointer[ i ] - 24 );
			heap->OutputFreeList();
			printf( "\n" );
		}
	}
	heap->OutputFreeList();
	
	delete heap;
	//scanf("%d", &stopflag );
	return 0;
}
