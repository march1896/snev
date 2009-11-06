#include "heap2.h"
#include <cstdlib>
#include <cstring>

#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( ALIGN_T - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + ALIGN_T - 1 ) & ~( ALIGN_T - 1 ) )

#define SPLIT_THRESHOLD 64
//¡¡¡¡ 			  ©³©·¡¡¡¡¡¡©³©·
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©³©¿©ß©¥©¥©¥©¿©ß©·
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§ ¡¡
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡©¥¡¡¡¡¡¡©§
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡©×©¿¡¡©»©×¡¡©§
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡©ß¡¡¡¡¡¡©§
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡©»©¥©·¡¡¡¡¡¡©³©¥©¿
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡©§¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡©§
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡©»©¥©¥©¥©·
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡¡¡¡¡¡¡¡¡©Ç©·
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§¡¡¡¡¡¡¡¡¡¡¡¡¡¡©³©¿
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©»©·©·©³©¥©×©·©³©¿
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡©§©Ï©Ï¡¡©§©Ï©Ï
//¡¡¡¡¡¡¡¡¡¡¡¡¡¡ ¡¡¡¡ ©»©ß©¿¡¡©»©ß©¿

typedef struct _Block {
	_Block* pPrevMem;
	_Block* pNextMem;
	_Block* pPrevFree;
	_Block* pNextFree;

#ifdef _MEM_DEBUG_
	uint 	Line;
	const char* 	FileName;
#endif
}Block;
const size_t BLOCKSIZE = sizeof( Block );

inline size_t GetBlockSize( Block* b ) {
	return (char*)b->pNextMem - (char*)b;
}

inline size_t GetBlockDataSize( Block* b ) {
	return GetBlockSize( b ) - BLOCKSIZE;
}

// VC's log2 function
inline uint uint_log2( uint Value ) {
uint bit;

	//Ensure Value is non-zero
	if( Value == 0 ) return 0;

	//Use a binary chop algorithm to find bit
	bit = 0;

	if( ((uint)Value & 0xffff0000) != 0 ) {
		Value >>= 16;
		bit = 16;
	}

	if( (Value & 0xff00) != 0 ) {
		Value >>= 8;
		bit +=8;
	}

	if( (Value & 0x00f0) != 0 ) {
		Value >>= 4;
		bit +=4;
	}

	if( (Value & 0x000c) != 0 ) {
		Value >>= 2;
		bit +=2;
	}

	if( (Value & 0x0002) != 0 ) {
		bit++;
	}

	//Return bit number found
	return bit;
}


inline uint Mylog2( uint x ) {
	// assert( x > 0 );
	int ret = 31;
	while ( ( x & ( 0x00000001 << ret ) ) == 0 ) {
		ret --;
	}
	return ret;
}

// #define log2( x ) Mylog2( x )
#define log2( x ) uint_log2( x )
#define MemCopy( dest, source, size ) memcpy( dest, source, size )

Block* 	pEndSentinel;
Block* 	pStartSentinel;
void* 	pHeapStart;
Block*  pFreeList[32];
size_t 	AlignMent;

static inline void PushIntoFreeList( Block* b );
static inline void PopFromFreeList( Block* b );
static inline bool CheckPointer( Block*b, bool ForFree );
static inline bool CheckBlock( Block* b );
static inline bool IsBlockFree( Block* b );

size_t GetMemoryBlockSize( void* Mem ) {
	Block* b = (Block*)( (char*)Mem - BLOCKSIZE );
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

void* AllocateLowClean( size_t Size ) {
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

		}
		if ( b != NULL ) {
			// find
			break;
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
		void* addr = (char*)b + BLOCKSIZE + Size;
		addr = (void*)ALIGNUP( (uint)addr, AlignMent );

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;
		if ( b->pNextMem != pEndSentinel ) b->pNextMem->pPrevMem = new_b;
		b->pNextMem = new_b;

		PushIntoFreeList( new_b );
	}

	return (void*) ( (char*) b + BLOCKSIZE );
}

void* AllocateHighClean( size_t Size ) {
	if ( Size <= 0 ) return NULL;
	uint bit = 31;
	Block* b = NULL;

	// find a large enough block
	while ( bit > 0 ) {
		if ( pFreeList[ bit ] != NULL ) 
			break;
		bit --;
	}
	b = pFreeList[ bit ];

	if ( b == NULL ) return NULL;

	while ( b != NULL ) {
		if ( Size < GetBlockDataSize( b ) ) {
			break;
		}
		b = b->pNextFree;
	}
	if ( b == NULL ) return NULL;

	// split this block into two small blocks
	if ( GetBlockDataSize( b ) - Size < SPLIT_THRESHOLD ) {
		// don't split, just give this whole block to user
		PopFromFreeList( b );
	}
	else {
		PopFromFreeList( b );
		void* addr = (char*)b + BLOCKSIZE + Size;
		addr = (void*)ALIGNUP( (uint)addr, AlignMent );

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;
		if ( b->pNextMem != pEndSentinel ) b->pNextMem->pPrevMem = new_b;
		b->pNextMem = new_b;

		PushIntoFreeList( new_b );
	}

	return (void*) ( (char*) b + BLOCKSIZE );
}

void* ReAllocateClean( void* Mem, size_t Size ) {
	Block* b = (Block*)( (char*)Mem - BLOCKSIZE );

	// if the block's remained space is enough for the desired size, just return
	if ( GetBlockDataSize( b ) > Size ) return Mem;

	Block* next = b->pNextMem;
	// the next block is free, and the space is enough, so lucky
	if ( IsBlockFree( next ) && GetBlockSize( next ) + GetBlockDataSize( b ) > Size ) {
		PopFromFreeList( next );
		if ( next->pNextMem != pEndSentinel ) next->pNextMem->pPrevMem = b;
		b->pNextMem = next->pNextMem;

		return Mem;
	}

	// the worst condition, allocate a new mem and move data to that area
	void* new_m = (void*)AllocateLowClean( Size );
	MemCopy( new_m, Mem, GetBlockDataSize( b ) );
	FreeClean( Mem );

	return new_m;
}

void FreeClean( void* ToFree ) {
	Block* b = (Block*)( (char*)ToFree - BLOCKSIZE );

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

void PopFromFreeList( Block* b ) {
	if ( !CheckBlock( b ) ) return;

	uint size = GetBlockSize( b );
	uint bit = log2( size );

	Block* prev = b->pPrevFree;
	Block* next = b->pNextFree;

	if ( prev == pStartSentinel ) {
		// b is in the front of this list
		// assert( b == pFreeList[ bit ] );
		//if ( pFreeList[ bit ] != b ) {
			// TODO: report error
		//}

		pFreeList[ bit] = next;
		if ( next ) next->pPrevFree = pStartSentinel;
	}
	else {
		// b is in the end of the list
		prev->pNextFree = next;
		if ( next ) next->pPrevFree = prev;
	}
	
	b->pPrevFree = b->pNextFree = NULL;
	return;
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
	void* mem = AllocateLowClean( Size );
	Block* b = (Block*)( (char*)mem - BLOCKSIZE );
	b->Line = line;
	b->FileName = filename;
	return mem;
}
void* AllocateHighDebug( size_t Size, uint line, const char* filename ) {
	void* mem = AllocateHighClean( Size );
	Block* b = (Block*)( (char*)mem - BLOCKSIZE );
	b->Line = line;
	b->FileName = filename;
	return mem;
}
void* ReAllocateDebug( void* Mem, size_t Size, uint line, const char* filename ) {
	void* mem = ReAllocateClean( Mem, Size );
	Block* b = (Block*)( (char*)mem - BLOCKSIZE );
	b->Line = line;
	b->FileName = filename;
	return mem;
}

void FreeDebug( void* Mem, uint line, const char* filename ) {
	Block* b = (Block*)( (char*)Mem - BLOCKSIZE );
	b->Line = line;
	b->FileName = filename;
	FreeClean( Mem );
	return;
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
			printf( "\t\tBlock Addr 0x%08x\tSize %d\n", (uint)( (char*)b + BLOCKSIZE ), GetBlockSize( b ) );
			b = b->pNextFree;
		}
	}
}

void CheckLeakPoint() {
	Block* b = (Block*)pStartSentinel + 1;
	printf( "block size: %d\n", BLOCKSIZE );

	while ( b < pEndSentinel ) {
		if ( CheckBlock( b ) && !IsBlockFree( b ) ) {
#ifdef _MEM_DEBUG_
			printf( "Leak point, addr: 0x%08x\tline: %d\tfile %s\n", (uint)(b + BLOCKSIZE), b->Line, b->FileName );
#else 
			printf( "Leak point, addr: 0x%08x\tline: %d\tfile %s\n",(uint)(b + BLOCKSIZE), -1, "" );
#endif
		}
		b = b->pNextMem;
	}
	return;
}
