#include <heap_buddy.h>

#include <cstdlib>
#include <cstring>

/* implementation of the public interface */

heap_handle cb_heap_init_buddy	(void *buff, int size) {
}

void	cb_heap_destroy_buddy	(heap_handle pheap) {
}	

void*	cb_heap_alloc_buddy		(heap_handle pheap, int size) {
}

void	cb_heap_dealloc_buddy	(heap_handle pheap, void *buff) {
}

void	cb_heap_realloc_buddy	(heap_handle pheap, void *buff, int size) {
}

/* private function and variable of this file */

/* the below align macros just apply for align with exponents of 2 */
#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( ALIGN_T - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + ALIGN_T - 1 ) & ~( ALIGN_T - 1 ) )

#define SPLIT_THRESHOLD 64

typedef struct block_t {
	block_t* pPrevMem;
	block_t* pNextMem;
	block_t* pPrevFree;
	block_t* pNextFree;

#ifdef _MEM_DEBUG_
	unsigned int 	line;
	const char* 	file;
#endif
} Block;

typedef struct heap_t {
	Block* 	pEndSentinel;
	Block* 	pStartSentinel;
	void* 	pHeapStart;
	Block*  pFreeList[32];
	size_t 	AlignMent;
	int 	errorCode;
} Heap;

const size_t BLOCK_HEADER_SIZE = sizeof(Block);

static inline size_t GetBlockSize(Block* b) {
	return (char*)b->pNextMem - (char*)b;
}

static inline size_t GetBlockDataSize(Block* b) {
	return GetBlockSize(b) - BLOCK_HEADER_SIZE;
}

// faster log2
static inline unsigned int unsigned int_log2(unsigned int Value) {
	if(Value == 0) return 0;

	unsigned int bit = 0;

	if( ((unsigned int)Value & 0xffff0000) != 0 ) {
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

	return bit;
}

#define MemCopy( dest, source, size ) memcpy( dest, source, size )

// push a free block to heap, return true if successfully pushed the block into the heap
static inline bool PushIntoFreeList(Heap* h, Block* b);
// pop a given block from a heap, return true if successfully poped the block.
static inline bool PopFromFreeList(Heap*h, Block* b);
static inline bool CheckPointer( Block*b, bool ForFree );
static inline bool CheckBlock( Block* b );
static inline bool IsBlockFree( Block* b );

static size_t GetMemoryBlockSize(void* Mem) {
	Block* b = (Block*)( (char*)Mem - BLOCK_HEADER_SIZE );
	if ( CheckBlock( b ) ) {
		return GetBlockSize( b );
	}
	return 0;
}

static Heap* InitHeap(void *Buffer, size_t Size, unsigned int Alignment) {
	// use the first several bytes to store the information of the heap handler.
	int HeapSize = sizeof(Heap);

	// initialize pheap handler
	Heap* pheap = (Heap*)Buffer;
	pheap->pHeapStart	= Buffer;
	pheap->AlignMent	= Alignment;
	pheap->errorCode	= 0;
	for (int i = 0; i < 32; i ++) {
		pheap->pFreeList[i]	= NULL;
	}

	// add HeapSize to buffer since it's already used
	Buffer = (void*)((char*)Buffer + HeapSize);

	void* _start = (void*)ALIGNUP((unsigned int)Buffer, AlignMent);
	void* _end = (void*)ALIGNDOWN((unsigned int)((char*)Buffer + Size ), AlignMent);

	pheap->pStartSentinel	= (Block*)_start;
	pheap->pEndSentinel		= (Block*)_end;

	// TODO: since pStartSentinel is just a symbol for start, it does not need a real address.
	Block* b 	= (Block*)_start + 1;
	b->pPrevMem = NULL;
	b->pNextMem = pEndSentinel;

	PushIntoFreeList(pheap, b);

	return (void*)pheap;
}

void DeinitHeap(Heap* pheap) {
	// nothing to do
}

// if not enough memory to allocate, return NULL 
void* AllocateLowClean(Heap* pheap, size_t Size) {
	if (Size <= 0) return NULL;

	unsigned int bit = log2(Size);
	Block* b = NULL;

	while (bit < 32) {
		b = pheap->pFreeList[bit];
		if (b != NULL) {
			// try to find a suitable block in current list
			while (b != NULL) {
				if (Size < GetBlockDataSize(b)) {
					// find the block
					break;
				}
				b = b->pNextFree;
			}

		}
		if (b != NULL) {
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
	if (GetBlockDataSize( b ) - Size < SPLIT_THRESHOLD) {
		// don't split, just give this whole block to user
		bool succeed = PopFromFreeList(pheap, b);
		if (!succed) pheap->errorCode |= ERR_POP_FREE_BLOCK;
	}
	else {
		bool succeed = PopFromFreeList(pheap, b);
		if (!succed) pheap->errorCode |= ERR_POP_FREE_BLOCK;

		void* addr = (char*)b + BLOCK_HEADER_SIZE + Size;
		addr = (void*)ALIGNUP((unsigned int)addr, pheap->AlignMent);

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;

		b->pNextMem = new_b;
		if (b->pNextMem != pheap->pEndSentinel) b->pNextMem->pPrevMem = new_b;

		PushIntoFreeList(pheap, new_b);
	}

	return (void*) ((char*)b + BLOCK_HEADER_SIZE);
}

// allocate from big block, it is faster, but leave more fragment
void* AllocateHighClean(Heap* pheap, size_t Size) {
	if (Size <= 0) return NULL;

	unsigned int bit = 31;
	Block* b = NULL;

	// find a large enough block
	while (bit > 0) {
		if (pFreeList[bit] != NULL) 
			break;
		bit --;
	}
	b = pFreeList[bit];

	if (b == NULL) return NULL;

	while (b != NULL) {
		if (Size < GetBlockDataSize(b)) {
			break;
		}
		b = b->pNextFree;
	}
	if (b == NULL) return NULL;

	// split this block into two small blocks
	if (GetBlockDataSize(b) - Size < SPLIT_THRESHOLD) {
		// don't split, just give this whole block to user
		bool succeed = PopFromFreeList(pheap, b);
		if (!succed) pheap->errorCode |= ERR_POP_FREE_BLOCK;
	}
	else {
		PopFromFreeList( b );
		bool succeed = PopFromFreeList(pheap, b);
		if (!succed) pheap->errorCode |= ERR_POP_FREE_BLOCK;

		void* addr = (char*)b + BLOCK_HEADER_SIZE + Size;
		addr = (void*)ALIGNUP( (unsigned int)addr, AlignMent );

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;
		b->pNextMem = new_b;
		if (b->pNextMem != pheap->pEndSentinel) b->pNextMem->pPrevMem = new_b;

		PushIntoFreeList(pheap, new_b);
	}

	return (void*) ( (char*) b + BLOCK_HEADER_SIZE );
}

void* ReAllocateClean( void* Mem, size_t Size ) {
	Block* b = (Block*)( (char*)Mem - BLOCK_HEADER_SIZE );

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
	void* new_m = (void*)AllocateLowClean(pheap, Size);
	MemCopy( new_m, Mem, GetBlockDataSize( b ) );
	FreeClean( Mem );

	return new_m;
}

void FreeClean( void* ToFree ) {
	Block* b = (Block*)( (char*)ToFree - BLOCK_HEADER_SIZE );

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
	unsigned int bit = 31;
	while ( pFreeList[ bit ] == NULL ) {
		bit --;
	}

	b = pFreeList[ bit ];
	while ( b->pNextFree != NULL ) {
		b = b->pNextFree;
	}

	return GetBlockDataSize( b );
}

bool PushIntoFreeList(Heap* h, Block* b) {
	size_t b_size = GetBlockSize(b);
	unsigned int bit = log2(b_size);

	Block* pFree = h->pFreeList[bit];
	if (pFree == NULL) {
		b->pPrevFree = pStartSentinel;
		b->pNextFree = NULL;
		h->pFreeList[bit] = b;
	}
	else {
		Block* prev = NULL;
		// sort block in increasing order
		while (pFree && GetBlockSize(pFree) < b_size) {
			prev = pFree;
			pFree = pFree->pNextFree;
		}

		if ( prev == NULL ) {
			// in the front of the list
			pFree->pPrevFree = b;
			b->pNextFree = pFree;
			b->pPrevFree = pStartSentinel;
			h->pFreeList[bit] = b;
		}
		else {
			b->pPrevFree = prev;
			b->pNextFree = pFree;
			prev->pNextFree = b;
			if (pFree) pFree->pPrevFree = b;
		}
	}

	return true;
}

bool PopFromFreeList(Heap* h, Block* b) {
	if (!CheckBlock(h, b)) return false;

	unsigned int size = GetBlockSize( b );
	unsigned int bit = log2( size );

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
	return true;
}

static inline bool CheckPointer(Heap* h, Block*b, bool ForFree) {
	if (b == NULL) return true;
	if (!ForFree) {
		return b > h->pStartSentinel && b <= h->pEndSentinel;
	}
	else {
		return b >= h->pStartSentinel && b < h->pEndSentinel;
	}
}

static inline bool CheckBlock(Heap* h, Block* b) {
	if (b == NULL) return false;
	if (b <= h->pStartSentinel || b >= h->pEndSentinel) return false;

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
void* AllocateLowDebug( size_t Size, unsigned int line, const char* file ) {
	void* mem = AllocateLowClean(pheap, Size);
	Block* b = (Block*)( (char*)mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	return mem;
}
void* AllocateHighDebug( size_t Size, unsigned int line, const char* file ) {
	void* mem = AllocateHighClean( Size );
	Block* b = (Block*)( (char*)mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	return mem;
}
void* ReAllocateDebug( void* Mem, size_t Size, unsigned int line, const char* file ) {
	void* mem = ReAllocateClean( Mem, Size );
	Block* b = (Block*)( (char*)mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	return mem;
}

void FreeDebug( void* Mem, unsigned int line, const char* file ) {
	Block* b = (Block*)( (char*)Mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	FreeClean( Mem );
	return;
}
#endif

// TODO: remove this
#include <cstdio>
void DumpFreeList() {
	for ( unsigned int i = 0; i < 32; i ++ ) {
		printf( "FL %d", i );
		Block* b = pFreeList[i];

		if ( b == NULL ) {
			printf("\n");
			continue;
		}

		while ( b != NULL ) {
			printf( "\t\tBlock Addr 0x%08x\tSize %d\n", (unsigned int)( (char*)b + BLOCK_HEADER_SIZE ), GetBlockSize( b ) );
			b = b->pNextFree;
		}
	}
}

void CheckLeakPoint() {
	Block* b = (Block*)pStartSentinel + 1;
	printf( "block size: %d\n", BLOCK_HEADER_SIZE );

	while ( b < pEndSentinel ) {
		if ( CheckBlock( b ) && !IsBlockFree( b ) ) {
#ifdef _MEM_DEBUG_
			printf( "Leak point, addr: 0x%08x\tline: %d\tfile %s\n", (unsigned int)(b + BLOCK_HEADER_SIZE), b->line, b->file );
#else 
			printf( "Leak point, addr: 0x%08x\tline: %d\tfile %s\n",(unsigned int)(b + BLOCK_HEADER_SIZE), -1, "" );
#endif
		}
		b = b->pNextMem;
	}
	return;
}
