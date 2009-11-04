#include "heap2.h"
#include <cstdlib>

#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( ALIGN_T - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + ALIGN_T - 1 ) & ~( ALIGN_T - 1 ) )

#define SPLIT_THRESHOLD 32

typedef struct _Block {
	_Block* pPrevMem;
	_Block* pNextMem;
	_Block* pPrevFree;
	_Block* pNextFree;

#ifdef _MEM_DEBUG_
	uint 	Line;
	char* 	FileName;
#endif
}Block;

size_t GetBlockSize( Block* b ) {
	return (char*)b->pNextMem - (char*)b;
}

size_t GetBlockDataSize( Block* b ) {
	return GetBlockSize( b ) - sizeof( Block );
}

uint Mylog2( uint x ) {
	// assert( x > 0 );
	int ret = 31;
	while ( ( x & ( 0x00000001 << ret ) ) == 0 ) {
		ret --;
	}
	return ret;
}

#define log2( x ) Mylog2( x )

#define ASSIGNBLOCK( b, prev, next, prevfree, nextfree ) 	\
	{ 														\
		b->pPrevMem = prev; 								\
		b->pNextMem = next; 								\
		b->pPrevFree = prevfree; 							\
		b->pNextFree = nextfree; 							\
	}

Block* 	pEndSentinel;
Block* 	pStartSentinel;
void* 	pHeapStart;
Block*  pFreeList[32];
size_t 	AlignMent;

static void PushIntoFreeList( Block* b );
static void PopFromFreeList( Block* b );
static bool CheckPointer( Block*b, bool ForFree );
static bool CheckBlock( Block* b );
static bool IsBlockFree( Block* b );

size_t GetMemoryBlockSize( void* Mem ) {
	Block* b = (Block*)( (char*)Mem - sizeof( Block ) );
	if ( CheckBlock( b ) ) {
		return GetBlockSize( b );
	}
	return 0;
}

void InitHeap( void *Buffer, size_t Size, uint Alignment ) {
	pHeapStart = Buffer;
	AlignMent = Alignment;
	for ( uint i = 0; i < 32; i ++ ) pFreeList[i] = NULL;

	void* _start = (void*)ALIGNUP( (uint)Buffer, AlignMent );
	void* _end = (void*)ALIGNDOWN( (uint)((char*)Buffer + Size ), AlignMent );

	pStartSentinel = (Block*)_start;
	pEndSentinel = (Block*)_end;

	Block* b = (Block*)_start + 1;
	b->pPrevMem = NULL;
	b->pNextMem = pEndSentinel;

	PushIntoFreeList( b );
}

void DeinitHeap() {
	// give the memory back to system.
}

void* AllocateLow( size_t Size ) {
	if ( Size <= 0 ) return NULL;
	uint bit = log2( Size );
	Block* b = NULL;

	while ( bit < 32 ) {
		b = pFreeList[ bit ];
		if ( b != NULL ) {
			// try to find a suitable block in current list
			while ( b != NULL ) {
				if ( Size < GetBlockDataSize( b ) ) {
					// find the block
					break;
				}
				b = b->pNextFree;
			}

			if ( b != NULL ) {
				// find
				break;
			}
		}

		bit ++;
	}

	if ( b == NULL ) {
		// not found, allocate failed
		return NULL;
	}

	// split this block into two small blocks
	if ( GetBlockDataSize( b ) - Size < SPLIT_THRESHOLD ) {
		// don't split, just give this whole block to user
		PopFromFreeList( b );
	}
	else {
		PopFromFreeList( b );
		void* addr = (char*)b + sizeof( Block ) + Size;
		addr = (void*)ALIGNUP( (uint)addr, AlignMent );

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;
		if ( b->pNextMem != pEndSentinel ) b->pNextMem->pPrevMem = new_b;
		b->pNextMem = new_b;

		PushIntoFreeList( new_b );
	}
	b->pPrevFree = b->pNextFree = NULL;

	return (void*) ( (char*) b + sizeof( Block ) );
}

void* AllocateHigh( size_t Size ) {
}

void* ReAllocate( void* Mem, size_t Size ) {
}

void Free( void* ToFree ) {
	Block* b = (Block*)( (char*)ToFree - sizeof( Block ) );

	if ( !CheckBlock( b ) ) {
		// SomeThing wrong!!
		// TODO: report infomation to caller
	}
	Block* prev = b->pPrevMem;
	Block* next = b->pNextMem;

	if ( IsBlockFree( prev ) && IsBlockFree( next ) ) {
		// merge three parts
		// remove from freelist
		PopFromFreeList( prev );
		PopFromFreeList( next );

		if ( next->pNextMem != pEndSentinel ) next->pNextMem->pPrevMem = prev;
		prev->pNextMem = next->pNextMem;
		
		PushIntoFreeList( prev );
	}
	else if ( IsBlockFree( prev ) ) {
		// merge b and prev
		PopFromFreeList( prev );

		if ( next != pEndSentinel ) next->pPrevMem = prev;
		prev->pNextMem = next;

		PushIntoFreeList( prev );
	}
	else if ( IsBlockFree( next ) ) {
		// merge b and next
		PopFromFreeList( next );

		if ( next->pNextMem != pEndSentinel ) next->pNextMem->pPrevMem = b;
		b->pNextMem = next->pNextMem;

		PushIntoFreeList( b );
	}
	else {
		// insert into a free list
		PushIntoFreeList( b );
	}
}

size_t GetLargestFree() {
	Block* b;
	uint bit = 31;
	while ( pFreeList[ bit ] == NULL ) {
		bit --;
	}

	b = pFreeList[ bit ];
	while ( b->pNextFree != NULL ) {
		b = b->pNextFree;
	}

	return GetBlockDataSize( b );
}

void PushIntoFreeList( Block* b ) {
	size_t b_size = GetBlockSize( b );
	uint bit = log2( b_size );

	Block* pFree = pFreeList[ bit ];
	if ( pFree == NULL ) {
		b->pPrevFree = pStartSentinel;
		b->pNextFree = NULL;
		pFreeList[ bit ] = b;
	}
	else {
		Block* prev = NULL;
		while ( pFree && GetBlockSize( pFree ) < b_size ) {
			prev = pFree;
			pFree = pFree->pNextFree;
		}

		if ( prev == NULL ) {
			// in the front of the list
			pFree->pPrevFree = b;
			b->pNextFree = pFree;
			b->pPrevFree = pStartSentinel;
			pFreeList[ bit ] = b;
		}
		else {
			b->pPrevFree = prev;
			b->pNextFree = pFree;
			prev->pNextFree = b;
			if ( pFree ) pFree->pPrevFree = b;
		}
	}
}

#include <cstdio>
void PopFromFreeList( Block* b ) {
	if ( !CheckBlock( b ) ) return;

	uint size = GetBlockSize( b );
	uint bit = log2( size );

	Block* prev = b->pPrevFree;
	Block* next = b->pNextFree;

	if ( prev == pStartSentinel ) {
		// b is in the front of this list
		// assert( b == pFreeList[ bit ] );
		if ( pFreeList[ bit ] != b ) {
			// TODO: report error
			printf( "fatal error!" );
		}

		pFreeList[ bit] = next;
		if ( next ) next->pPrevFree = NULL;
	}
	else {
		// b is in the end of the list
		prev->pNextFree = next;
		if ( next ) next->pPrevFree = prev;
	}
}

bool CheckPointer( Block*b, bool ForFree ) {
	if ( b == NULL ) return true;
	if ( !ForFree ) {
		return b > pStartSentinel && b <= pEndSentinel;
	}
	else {
		return b >= pStartSentinel && b < pEndSentinel;
	}
}

bool CheckBlock( Block* b ) {
	if ( b == NULL ) return false;
	if ( b <= pStartSentinel || b >= pEndSentinel ) return false;

	bool ret = true;
	ret &= CheckPointer( b->pPrevMem, false );
	ret &= CheckPointer( b->pNextMem, false );
	ret &= CheckPointer( b->pPrevFree, true );
	ret &= CheckPointer( b->pNextFree, true );

	return ret;
}

bool IsBlockFree( Block* b ) {
	if ( !CheckBlock( b ) ) return false;

	if ( b->pPrevFree || b->pNextFree ) return true;
	return false;
}

#ifdef _MEM_DEBUG_
void* AllocateLowDebug( size_t Size, uint line, const char* filename ) {
}
void* AllocateHighDebug( size_t Size, uint line, const char* filename ) {
}
void* ReAllocateDebug( void* Mem, size_t Size, uint line, const char* filename ) {
}
void FreeDebug( void* Mem, uint line, const char* filename ) {
}
#endif

// TODO: remove this
#include <cstdio>
void DumpFreeList() {
	for ( uint i = 0; i < 32; i ++ ) {
		printf( "FL %d", i );
		Block* b = pFreeList[i];

		if ( b == NULL ) {
			printf("\n");
			continue;
		}

		while ( b != NULL ) {
			printf( "\t\tBlock Addr 0x%08x\tSize %d\n", (uint)( (char*)b + sizeof( Block ) ), GetBlockSize( b ) );
			b = b->pNextFree;
		}
	}
}
