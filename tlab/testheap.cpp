#include "heap.h"
#include "heap2.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <windows.h>

double getTime2() //使用高精度计时器
{      
	static LARGE_INTEGER s_freq;
	LARGE_INTEGER performanceCount;
	double t;
	if (s_freq.QuadPart==0)
	{
		if ( !QueryPerformanceFrequency( &s_freq))
			return 0;
	}

	QueryPerformanceCounter( &performanceCount );
	t=(double)performanceCount.QuadPart / (double)s_freq.QuadPart;
	return t;
}


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
	double t0;

	void* memory = (void*)malloc( 2 << 20 );
	InitHeap( memory, 2<<20 );

	//DumpFreeList();

	const int SIZE = 10000;
	const int NUM = 1000000;
	char* pointer[ SIZE ];
	memset( pointer, 0, sizeof( char* )*SIZE );

	t0 = getTime2();

	for ( int i = 0; i < NUM; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % 300 + 1;
			//printf( "Allocate: %d\t\t", size );
			
			pointer[ num ] = (char*)Allocate( size * sizeof( char ) );
			if ( pointer[ num ] != NULL ) {
				//printf( "Pointer %d \tallocate %d, \taddr 0x%08x\n", num, size, (unsigned)pointer[ num ] );
			}
		}
		else {
			//printf( "Free:\t\t" );
			Free( pointer[num] );
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", num, GetMemoryBlockSize( pointer[ num] ), (unsigned)pointer[ num ] - 16 );
			pointer[ num ] = 0;
		}
		//DumpFreeList();
		//heap->OutputFreeList();
		//printf( "\n" );
	}

	for ( int i = 0; i < SIZE; i ++ ) {
		if ( pointer[ i ] ) {
			Free( pointer[ i ] );
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", i, GetMemoryBlockSize( pointer[ i ] ), (unsigned)pointer[ i ] - 16 );
			//DumpFreeList();
			//printf( "\n" );
		}
	}
	//DumpFreeList();

	printf( "heap2 use %lf\n", getTime2() - t0 );
	t0 = getTime2();

	free( memory );

	CHeap1* heap = new CHeap1();
	memory = (void*)malloc( 2 << 20 );
	heap->CreateFromBuffer( memory, 2 << 20 );

	heap->OutputFreeList();
	printf( "\n" );

	memset( pointer, 0, sizeof( char* )*SIZE );

	for ( int i = 0; i < NUM; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % 300 + 1;
			
			pointer[ num ] = (char*)heap->Alloc( size * sizeof( char ) );
			if ( pointer[ num ] != NULL ) {
				//printf( "Pointer %d \tallocate %d, \taddr 0x%08x\n", num, size, (unsigned)pointer[ num ] );
			}
		}
		else {
			heap->Free( pointer[num] );
			pointer[ num ] = 0;
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", num, heap->GetBlockSize( pointer[ num] ), (unsigned)pointer[ num ] - 24 );
		}
		//heap->OutputFreeList();
		//printf( "\n" );
	}

	printf( "End\n\n\n" );
	//heap->OutputFreeList();

	for ( int i = 0; i < SIZE; i ++ ) {
		if ( pointer[ i ] ) {
			heap->Free( pointer[ i ] );
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", i, heap->GetBlockSize( pointer[ i ] ), (unsigned)pointer[ i ] - 24 );
			//heap->OutputFreeList();
			//printf( "\n" );
		}
	}
	heap->OutputFreeList();

	delete heap;

	printf( "heap use %lf\n", getTime2() - t0 );
	t0 = getTime2();

	memset( pointer, 0, sizeof( char* )*SIZE );

	for ( int i = 0; i < NUM; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % 300 + 1;
			
			pointer[ num ] = (char*)malloc( size * sizeof( char ) );
			if ( pointer[ num ] != NULL ) {
			}
		}
		else {
			free( pointer[num] );
			pointer[ num ] = NULL;
		}
	}


	for ( int i = 0; i < SIZE; i ++ ) {
		if ( pointer[ i ] ) {
			free( pointer[ i ] );
		}
	}
	printf( "system heap use %lf\n", getTime2() - t0 );
	
	//scanf("%d", &stopflag );
	return 0;
}
