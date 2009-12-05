#include <windows.h>
//#define _MEM_DEBUG_
//#define _REDIRECT_NEW_
//#include "heap.h"
#include "heap2.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

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

/*
void testTime() {
	double t0;

	const int INIT_MEMORY_SIZE = 1 << 29;
	void* memory = (void*)malloc( INIT_MEMORY_SIZE );
	if ( memory == NULL ) printf( "!!!!!FATAL ERROR!!!!!\n" );
	InitHeap( memory, INIT_MEMORY_SIZE );

	//DumpFreeList();

	const int SIZE 	= 100000;
	const int NUM 	= 1000000; // test num
	const int RANDOM_ALLOC_MAX_SIZE = 10000;
	char* pointer[ SIZE ];
	memset( pointer, 0, sizeof( char* )*SIZE );

	t0 = getTime2();

	for ( int i = 0; i < NUM; i ++ ) {
		int num = rand() % SIZE;
		//printf( "Start num = %d\n", num );
		//printf( "pointer addr: 0x%08x\n", (uint)pointer[num] );
		if ( pointer[ num ] == NULL ) {
			int size = rand() % RANDOM_ALLOC_MAX_SIZE + 1;
			//printf( "Allocate: %d\t\t", size );
			//printf( "here, fuck\n" );
			
			pointer[ num ] = (char*)Allocate( size * sizeof( char ) );
			if ( pointer[ num ] == NULL ) {
				printf( "Allocate failed\n" );
				//printf( "Pointer %d \tallocate %d, \taddr 0x%08x\n", num, size, (unsigned)pointer[ num ] );
			}
		}
		else {
			//printf( "Free:\t\t" );
			Free( pointer[num] );
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", num, GetMemoryBlockSize( pointer[ num] ), (unsigned)pointer[ num ] - 16 );
			pointer[ num ] = NULL;
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
	memory = (void*)malloc( INIT_MEMORY_SIZE );
	if ( memory == NULL ) printf( "!!!!!FATAL ERROR!!!!!\n" );
	heap->CreateFromBuffer( memory, INIT_MEMORY_SIZE );

	//heap->OutputFreeList();
	printf( "\n" );

	memset( pointer, 0, sizeof( char* )*SIZE );

	for ( int i = 0; i < NUM; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % RANDOM_ALLOC_MAX_SIZE + 1;
			
			pointer[ num ] = (char*)heap->Alloc( size * sizeof( char ) );
			if ( pointer[ num ] == NULL ) {
				printf( "Allocate failed\n" );
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

	//printf( "End\n\n\n" );
	//heap->OutputFreeList();

	for ( int i = 0; i < SIZE; i ++ ) {
		if ( pointer[ i ] ) {
			heap->Free( pointer[ i ] );
			//printf( "Pointer %d \tfree %d, \taddr 0x%08x\n", i, heap->GetBlockSize( pointer[ i ] ), (unsigned)pointer[ i ] - 24 );
			//heap->OutputFreeList();
			//printf( "\n" );
		}
	}
	//heap->OutputFreeList();

	delete heap;

	printf( "heap use %lf\n", getTime2() - t0 );
	t0 = getTime2();

	memset( pointer, 0, sizeof( char* )*SIZE );

	for ( int i = 0; i < NUM; i ++ ) {
		int num = rand() % SIZE;
		if ( pointer[ num ] == 0 ) {
			int size = rand() % RANDOM_ALLOC_MAX_SIZE + 1;
			
			pointer[ num ] = (char*)malloc( size * sizeof( char ) );
			if ( pointer[ num ] == NULL ) {
				printf( "Allocate failed\n" );
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
}
*/

class A {
public:
	A() {
		printf( "A constructed\n" );
	}
	~A() {
		printf( "A deconstructed\n" );
	}
	int data[10];
};

class B {
public: 
	B() {
		printf( "B constructed\n" );
	}
	~B() {
		printf( "B deconstructed\n" );
	}
	A a[3];
	char c[10];
};

void testHeapAdapter() {
	//printf( "%d\t%s\n", __LINE__, __FILE__ );
	const int INIT_MEMORY_SIZE = 1 << 20;
	void* memory = (void*)malloc( INIT_MEMORY_SIZE );
	if ( memory == NULL ) printf( "!!!!!FATAL ERROR!!!!!\n" );
	InitHeap( memory, INIT_MEMORY_SIZE );
	//DumpFreeList();

	printf( "hello, world\n" );

	A* a = new A();
	B* b = new B();
	printf( "\nA & B are constructed\n" );

	//delete a;
	delete b;

	CheckLeakPoint();
	//DumpFreeList();

	free( memory );
}

int main() {
	int stopflag;

	testHeapAdapter();
	//scanf("%d", &stopflag );
	return 0;
}
