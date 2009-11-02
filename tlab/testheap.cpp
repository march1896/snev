#include "heap.h"
#include <cstdio>
#include <cstdlib>

int main() {
	int stopflag;
	CHeap1* heap = new CHeap1();
	void *memory = (void*)malloc( 2 << 16 );
	heap->CreateFromBuffer( memory, 2 << 16 );

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
	
	delete heap;
	scanf("%d", &stopflag );
	return 0;
}
