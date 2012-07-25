#include <heap_buddy.h>

#include <cstdlib>
#include <cstring>

#define DEFAULT_ALIGNMENT 32
typedef struct heap_t Heap;


/* the below align macros just apply for align with exponents of 2 */
#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( (ALIGN_T) - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + (ALIGN_T) - 1 ) & ~( (ALIGN_T) - 1 ) )

#define SPLIT_THRESHOLD 64

/* error code related definitions */
#define ERR_HEAP_INIT			(1<<0)
#define ERR_HEAP_DEINIT			(1<<1)
#define ERR_HEAP_ALLOCATE		(1<<2)
#define ERR_HEAP_DEALLOCATE		(1<<3)
#define ERR_HEAP_REALLOCATE		(1<<4)
#define ERR_HEAPINTERNAL_ALLOCLOW		(1<<10)
#define ERR_HEAPINTERNAL_ALLOCHIGH		(1<<11)
#define ERR_HEAPINTERNAL_PUSHFREE		(1<<12)
#define ERR_HEAPINTERNAL_POPFREE		(1<<13)
#define ERR_HEAPINTERNAL_CHECKBLOCK		(1<<14)

#define ERR_HEAP_MASK \
	(ERR_HEAP_INIT | ERR_HEAP_DEINIT | ERR_HEAP_ALLOCATE | ERR_HEAP_DEALLOCATE | ERR_HEAP_REALLOCATE)

#define ERR_HEAPINTERNAL_MASK \
	(ERR_HEAPINTERNAL_ALLOCLOW | ERR_HEAPINTERNAL_ALLOCHIGH | ERR_HEAPINTERNAL_PUSHFREE | \
	ERR_HEAPINTERNAL_POPFREE | ERR_HEAPINTERNAL_CHECKBLOCK  )

#define GetInternalErr(h) ((h->errorCode) & ERR_HEAPINTERNAL_MASK)

#define RaiseError(h, err) ((h->errorCode) |= err)

/* Heap and Block definitions */
/* design requirements for struct block:
 *
 * when the block is allocated, it is not in the free list, we should know:
 *    1, the previous/next adjacent block. (when it is given back to the heap
 *       it should know the preivous/next block address to do the combine work).
 *
 * when the block is not allocated, it is in the free list, we should know:
 *    1, the size of the block. (in order to deside if it's allocatable)
 *    2, the previous/next free block. (to maintain the free list)
 *    3, the next adjacent block,(it is a little hard to understand this 
 *       rule, consider the current block is in free list, and the prev/next 
 *       adjacent blocks are both allocated, and now the prev adjacent freed and 
 *       combined with the current block, now the next block is still allocated,
 *       so it should know the previous adjacent block, which will be changed 
 *       after the combination, so we should notify the next adjacent block that
 *       its prev is changed, the prev adjacent block is no need to know since its 
 *       next pointer will never changed if it's allocated), this rule is duplicated
 *       with rule 1 since if we know the address and size of a block, we will 
 *       certainly know the next adjacent block.
 *
 *  at any time, when a get a pointer of a block, we should tell if the block is in
 *    free list. (this is also related for combination work, when we try to combine 
 *    the current block with prev/next adjacent block, we should first prev/next 
 *    adjacent blocks are in free list)
 * 	
 * 	conclusion:
 * 	for allocated block, we should keep the header information as few as possible,
 * 	from the header we could get the prev/next adjacent block, and if the block is
 * 	allocated.
 *
 * 	for in freelist block, the header size is not sensitive, we could save a lot of
 * 	information in the header.
 */

struct block_h {
	/* use prev_adj to judge if the block is allocated or in freelist.
	 * since if the block is in freelist, prev_adj is not useful, so we can set to 
	 * any value we want.
	 * prev_adj == self_addr represent the block is in freelist 
	 * fields for allocated blocks start. */
	struct block_h* prev_adj; 
	struct block_h* next_adj;
	/* fields for allocated blocks end. */

	/* these fields of data stucture is the key of managing free blocks, 
	 * changing algorithm only impacts on these fields */
	struct block_h* prev_free;
	struct block_h* next_free;

	/* below is information for check and debug */
	struct heap_t*  onwer;
	const char* 	file;
	const int		line;
};

typedef struct block_h block;

/* this is a dummy struct, the only reason it's defined is to get the allocated 
 * block header size, DON'T use it directly */
struct __block_allocated_h {
	struct __block_allocated_h* prev_adj;
	struct __block_allocated_h* next_adj;
};

const int BLOCK_ALLOCATED_SIZE = sizeof(__block_allocated_h);

/* block_com_ series are common functions for kinds of algorithm */

// return true if pb is in free list 
inline bool block_com_isfree(block* pb) {
	// see the data definition.
	if (pb == pb->prev_free) return true;
	return false;
}

// mark pb as it is in free list
inline void block_com_markfree(block* pb) {
	pb->prev_free = pb;
}

// return the data address of the pb
inline void* block_com_get_data(block* pb) {
	return (void*)((char*)pb + BLOCK_ALLOCATED_SIZE);
}

inline block* block_com_from_data(void* addr) {
	return (block*)((char*)addr - BLOCK_ALLOCATED_SIZE);
}

inline int block_com_data_size(block* pb) {
	return (char*)pb->next_adj - (char*)pb - BLOCK_ALLOCATED_SIZE;
}

inline int block_com_merge_two(block* pf, block* pe) {
}

inline int block_com_merge_three(block *pf, block *pm, block *pe) {
}

/* use double linked list(dll) to manage free list, block_dll_ implement 
 * related functions */

// find a suitable block of given size.
inline block* block_dll_find(block* phead, int size) {
}

inline void block_dll_insert(block **pphead, block* pin) {
}

inline void block_dll_erase(block **pphead, block* pre) {
}

/********************************************************************************
 
  *****  heap memory distribution ****
                                                                                  
  		heap header	                     block buffer
      |-----------|--|----------------------------------------------|------|--|
    pbuff            ^                                              ^      ^      
                     |                                              |      |      
                block_start                                         |   block_end    
                                                              block_lastvalid     
                                                                                  
    the small gaps indicates align gaps                                           

*********************************************************************************
                                                                                  
  *****  block memory distribution ****
                                                                                  
        prev block               current block                        next block  
      |--------------|----------------|-----------------------------|---------|
                     ^ block header   ^      block data                           
                   pblock          data begin                                     
                                                                                  
                                                                                  
*********************************************************************************/

typedef struct heap_t {
	void*	pbuff;
	int		size;

	block*	block_start;	// points to the start of the block buffer
	block*	block_end;		// points to the end of the block buffer
	block*	block_lastvalid;	// points to the last valid block in the block boffer
	block*  block_free[32];

	int		alignment;
	int 	flag;
	int 	error;
} heap;



heap* heap_init(void* buff, int size) {
	
	int HeapSize = sizeof(Heap);

	// initialize pheap handler
	Heap* pheap = (Heap*)Buffer;
	pheap->pHeapStart	= Buffer;
	pheap->AlignMent	= Alignment;
	pheap->errorCode	= 0;
	for (int i = 0; i < 32; i ++) {
		pheap->pFreeList[i]	= NULL;
	}

	// add HeapSize to buffer since it's used by the heap header
	Buffer = (void*)((char*)Buffer + HeapSize);

	void* _start = (void*)ALIGNUP((unsigned int)Buffer, Alignment);
	void* _end = (void*)ALIGNDOWN((unsigned int)((char*)Buffer + Size ), Alignment);

	pheap->pStartSentinel	= (Block*)_start;
	pheap->pEndSentinel		= (Block*)_end;

	Block* b 	= (Block*)_start + 1;
	b->pPrevMem = NULL;
	b->pNextMem = pheap->pEndSentinel;

	bool succeed = PushIntoFreeList(pheap, b);
	if (!succeed) RaiseError(pheap, ERR_HEAP_INIT);

	return pheap;
}

void heap_deinit(heap* ph) {
}

void* heap_alloc(heap* ph, int size) {
}

void heap_dealloc(heap* ph, void* addr) {
}

void heap_realloc(heap* ph, void* addr, int nsize) {
}


const size_t BLOCK_HEADER_SIZE = sizeof(Block);

static inline size_t GetBlockSize(Block* b) {
	return (char*)b->pNextMem - (char*)b;
}

static inline size_t GetBlockDataSize(Block* b) {
	return GetBlockSize(b) - BLOCK_HEADER_SIZE;
}

// faster log2
static inline unsigned int int_log2(unsigned int Value) {
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

#define log2 int_log2
#define MemCopy( dest, source, size ) memcpy( dest, source, size )

// push a free block to heap, return true if successfully pushed the block into the heap
static bool PushIntoFreeList(Heap* h, Block* b);
// pop a given block from a heap, return true if successfully poped the block.
static bool PopFromFreeList(Heap*h, Block* b);
// return true if the block is validate
static bool CheckPointer(Heap* h, Block*b, bool ForFree);
static bool CheckBlock(Heap* h, Block* b );
static bool IsBlockFree(Block* b);

// static size_t GetMemoryBlockSize(void* Mem) {
// 	Block* b = (Block*)( (char*)Mem - BLOCK_HEADER_SIZE );
// 	if ( CheckBlock( b ) ) {
// 		return GetBlockSize( b );
// 	}
// 	return 0;
// }

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

	// add HeapSize to buffer since it's used by the heap header
	Buffer = (void*)((char*)Buffer + HeapSize);

	void* _start = (void*)ALIGNUP((unsigned int)Buffer, Alignment);
	void* _end = (void*)ALIGNDOWN((unsigned int)((char*)Buffer + Size ), Alignment);

	pheap->pStartSentinel	= (Block*)_start;
	pheap->pEndSentinel		= (Block*)_end;

	Block* b 	= (Block*)_start + 1;
	b->pPrevMem = NULL;
	b->pNextMem = pheap->pEndSentinel;

	bool succeed = PushIntoFreeList(pheap, b);
	if (!succeed) RaiseError(pheap, ERR_HEAP_INIT);

	return pheap;
}

static void DeinitHeap(Heap* pheap) {
	// nothing to do
}

// if not enough memory to allocate, return NULL 
static void* AllocateLowClean(Heap* pheap, size_t Size) {
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

	int interr = GetInternalErr(pheap);
	// split this block into two small blocks
	if (GetBlockDataSize( b ) - Size < SPLIT_THRESHOLD) {
		// don't split, just give this whole block to user
		PopFromFreeList(pheap, b);
	}
	else {
		PopFromFreeList(pheap, b);

		void* addr = (char*)b + BLOCK_HEADER_SIZE + Size;
		addr = (void*)ALIGNUP((unsigned int)addr, pheap->AlignMent);

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;

		b->pNextMem = new_b;
		if (b->pNextMem != pheap->pEndSentinel) b->pNextMem->pPrevMem = new_b;

		PushIntoFreeList(pheap, new_b);
	}

	if (interr != GetInternalErr(pheap)) 
		RaiseError(pheap, ERR_HEAP_ALLOCATE | ERR_HEAPINTERNAL_ALLOCLOW);

	return (void*) ((char*)b + BLOCK_HEADER_SIZE);
}

// allocate from big block, it is faster, but leave more fragment
static void* AllocateHighClean(Heap* pheap, size_t Size) {
	if (Size <= 0) return NULL;

	unsigned int bit = 31;
	Block* b = NULL;

	// find a large enough block
	while (bit > 0) {
		if (pheap->pFreeList[bit] != NULL) 
			break;
		bit --;
	}
	b = pheap->pFreeList[bit];

	if (b == NULL) return NULL;

	while (b != NULL) {
		if (Size < GetBlockDataSize(b)) {
			break;
		}
		b = b->pNextFree;
	}
	if (b == NULL) return NULL;

	int interr = GetInternalErr(pheap);
	// split this block into two small blocks
	if (GetBlockDataSize(b) - Size < SPLIT_THRESHOLD) {
		// don't split, just give this whole block to user
		PopFromFreeList(pheap, b);
	}
	else {
		PopFromFreeList(pheap, b);
		PopFromFreeList(pheap, b);

		void* addr = (char*)b + BLOCK_HEADER_SIZE + Size;
		addr = (void*)ALIGNUP( (unsigned int)addr, pheap->AlignMent );

		Block* new_b = (Block*)addr;

		new_b->pPrevMem = b;
		new_b->pNextMem = b->pNextMem;
		b->pNextMem = new_b;
		if (b->pNextMem != pheap->pEndSentinel) b->pNextMem->pPrevMem = new_b;

		PushIntoFreeList(pheap, new_b);
	}

	if (interr != GetInternalErr(pheap)) 
		RaiseError(pheap, ERR_HEAP_ALLOCATE | ERR_HEAPINTERNAL_ALLOCLOW);

	return (void*) ((char*)b + BLOCK_HEADER_SIZE);
}

static void* ReAllocateClean(Heap* pheap, void* Mem, size_t Size) {
	Block* b = (Block*)((char*)Mem - BLOCK_HEADER_SIZE);

	// if the block's remained space is enough for the desired size, just return
	if (GetBlockDataSize( b ) > Size) return Mem;

	int interr = GetInternalErr(pheap);

	Block* next = b->pNextMem;
	void* new_addr = NULL;
	if (IsBlockFree(next) && GetBlockSize(next) + GetBlockDataSize(b) >= Size) {
		// the next block is free, and the space is enough, so lucky
		PopFromFreeList(pheap, next);
		b->pNextMem = next->pNextMem;
		if (next->pNextMem != pheap->pEndSentinel) next->pNextMem->pPrevMem = b;

		new_addr = Mem;
	}
	else {
		// the worst condition, allocate a new mem and move data to that area
		new_addr = (void*)AllocateLowClean(pheap, Size);
		MemCopy(new_addr, Mem, GetBlockDataSize(b));
		FreeClean(pheap, Mem);
	}
	if (interr != GetInternalErr(pheap)) 
		RaiseError(pheap, ERR_HEAP_REALLOCATE);

	return new_addr;
}

static void FreeClean(Heap* pheap, void* ToFree) {
	Block* b = (Block*)((char*)ToFree - BLOCK_HEADER_SIZE);

	if (!CheckBlock(pheap, b)) {
		RaiseError(pheap, ERR_HEAP_DEALLOCATE);
	}

	int interr = GetInternalErr(pheap);

	Block* prev = b->pPrevMem;
	Block* next = b->pNextMem;

	if (IsBlockFree(prev) && IsBlockFree(next)) {
		// merge three parts
		// remove from free list
		PopFromFreeList(pheap, prev);
		PopFromFreeList(pheap, next);

		if ( next->pNextMem != pheap->pEndSentinel ) next->pNextMem->pPrevMem = prev;
		prev->pNextMem = next->pNextMem;
		
		PushIntoFreeList(pheap, prev);
	}
	else if (IsBlockFree(prev)) {
		// merge b and prev
		PopFromFreeList(pheap, prev);

		if (next != pheap->pEndSentinel) next->pPrevMem = prev;
		prev->pNextMem = next;

		PushIntoFreeList(pheap, prev);
	}
	else if (IsBlockFree(next)) {
		// merge b and next
		PopFromFreeList(pheap, next);

		if (next->pNextMem != pheap->pEndSentinel) next->pNextMem->pPrevMem = b;
		b->pNextMem = next->pNextMem;

		PushIntoFreeList(pheap, b);
	}
	else {
		// insert into a free list
		PushIntoFreeList(pheap, b);
	}

	if (interr != GetInternalErr(pheap)) 
		RaiseError(pheap, ERR_HEAP_DEALLOCATE);

	return;
}

static size_t GetLargestFree(Heap* pheap) {
	Block* b = NULL;
	unsigned int bit = 31;

	while (pheap->pFreeList[bit] == NULL) {
		bit --;
	}

	b = pheap->pFreeList[bit];
	while (b->pNextFree != NULL) {
		b = b->pNextFree;
	}

	return b == NULL ? 0 : GetBlockDataSize(b);
}

static bool PushIntoFreeList(Heap* h, Block* b) {
	size_t b_size = GetBlockSize(b);
	unsigned int bit = log2(b_size);

	Block* pFree = h->pFreeList[bit];
	if (pFree == NULL) {
		b->pPrevFree = h->pStartSentinel;
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
			b->pPrevFree = h->pStartSentinel;
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
	if (!CheckBlock(h, b)) {
		RaiseError(h, ERR_HEAPINTERNAL_POPFREE);
		return false;
	}

	unsigned int size = GetBlockSize(b);
	unsigned int bit = log2(size);

	Block* prev = b->pPrevFree;
	Block* next = b->pNextFree;

	if (prev == h->pStartSentinel) {
		// b is in the front of this list
		if (h->pFreeList[ bit ] != b ) {
			RaiseError(h, ERR_HEAPINTERNAL_POPFREE);
		}

		h->pFreeList[bit] = next;
		if (next) next->pPrevFree = h->pStartSentinel;
	}
	else {
		// b is in the middle or in end of the list
		prev->pNextFree = next;
		if (next) next->pPrevFree = prev;
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
	ret &= CheckPointer(h, b->pPrevMem, false);
	ret &= CheckPointer(h, b->pNextMem, false);
	ret &= CheckPointer(h, b->pPrevFree, true);
	ret &= CheckPointer(h, b->pNextFree, true);

	if (!ret) RaiseError(h, ERR_HEAPINTERNAL_CHECKBLOCK);
	return ret;
}

static bool IsBlockFree(Block* b) {
	// if (!CheckBlock( b )) return false;
	if (b == NULL) return false;

	if (b->pPrevFree || b->pNextFree) return true;
	return false;
}

#ifdef MEMORY_DEBUG
static void* AllocateLowDebug(Heap* pheap, size_t Size, unsigned int line, const char* file ) {
	void* mem = AllocateLowClean(pheap, Size);
	Block* b = (Block*)( (char*)mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	return mem;
}

static void* AllocateHighDebug(Heap* pheap, size_t Size, unsigned int line, const char* file ) {
	void* mem = AllocateHighClean(pheap, Size);
	Block* b = (Block*)( (char*)mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	return mem;
}

static void* ReAllocateDebug(Heap* pheap, void* Mem, size_t Size, unsigned int line, const char* file ) {
	void* mem = ReAllocateClean(pheap, Mem, Size);
	Block* b = (Block*)( (char*)mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	return mem;
}

static void FreeDebug(Heap* pheap, void* Mem, unsigned int line, const char* file ) {
	Block* b = (Block*)( (char*)Mem - BLOCK_HEADER_SIZE );
	b->line = line;
	b->file = file;
	FreeClean(pheap, Mem);
	return;
}
#endif // MEMORY_DEBUG

#ifndef MEMORY_DEBUG

/* implementation of the public interface */
heap_handle cb_heap_init_buddy(void *buff, int size) {
	return (heap_handle)InitHeap(buff, (size_t)size, DEFAULT_ALIGNMENT);
}

void cb_heap_destroy_buddy(heap_handle pheap) {
	DeinitHeap((Heap*)pheap);
}	

void* cb_heap_alloc_buddy(heap_handle pheap, int size, const char* file, size_t line) {
	return AllocateLowClean((Heap*)pheap, (size_t)size);
}

void cb_heap_dealloc_buddy(heap_handle pheap, void *buff, const char* file, size_t line) {
	FreeClean((Heap*)pheap, buff);
}

void* cb_heap_realloc_buddy(heap_handle pheap, void *buff, int size, const char* file, size_t line) {
	return ReAllocateClean((Heap*)pheap, buff, (size_t)size);
}

void cb_heap_dump_buddy(heap_handle pheap) {
	return;
}

#else // MEMORY_DEBUG

static void* 	AllocateLowDebug(Heap* pheap, size_t Size, unsigned int line, const char* file);
static void* 	AllocateHighDebug(Heap* pheap, size_t Size, unsigned int line, const char* file);
static void* 	ReAllocateDebug(Heap* pheap, void* Mem, size_t Size, unsigned int line, const char* file);
static void 	FreeDebug(Heap* pheap, void* Mem, unsigned int line, const char* file);

heap_handle cb_heap_init_buddy_debug(void *buff, int size) {
	return (heap_handle)InitHeap(buff, (size_t)size, DEFAULT_ALIGNMENT);
}

void cb_heap_destroy_buddy_debug(heap_handle pheap) {
	DeinitHeap((Heap*)pheap);
}

void* cb_heap_alloc_buddy_debug(heap_handle pheap, int size, const char* file, size_t line) {
	return AllocateLowDebug((Heap*)pheap, (size_t)size, line, file);
}

void cb_heap_dealloc_buddy_debug(heap_handle pheap, void *buff, const char* file, size_t line) {
	FreeDebug((Heap*)pheap, buff, line, file);
}

void* cb_heap_realloc_buddy_debug(heap_handle pheap, void *buff, int size, const char* file, size_t line) {
	return ReAllocateDebug((Heap*)pheap, buff, (size_t)size, line, file);
}

void cb_heap_dump_buddy_debug(heap_handle pheap) {
}

#endif // MEMORY_DEBUG

void fill_heap_operations_buddy (heap_operations* ops) {
#ifndef MEMORY_DEBUG
	ops->init = cb_heap_init_buddy;
	ops->destroy = cb_heap_destroy_buddy;
	ops->alloc = cb_heap_alloc_buddy;
	ops->dealloc = cb_heap_dealloc_buddy;
	ops->realloc = cb_heap_realloc_buddy;
	ops->dump = cb_heap_dump_buddy;
#else // MEMORY_DEBUG
	ops->init = cb_heap_init_buddy_debug;
	ops->destroy = cb_heap_destroy_buddy_debug;
	ops->alloc = cb_heap_alloc_buddy_debug;
	ops->dealloc = cb_heap_dealloc_buddy_debug;
	ops->realloc = cb_heap_realloc_buddy_debug;
	ops->dump = cb_heap_dump_buddy_debug;
#endif // MEMORY_DEBUG
}

// TODO: remove this
// #include <cstdio>
// void DumpFreeList() {
// 	for ( unsigned int i = 0; i < 32; i ++ ) {
// 		printf( "FL %d", i );
// 		Block* b = pFreeList[i];
// 
// 		if ( b == NULL ) {
// 			printf("\n");
// 			continue;
// 		}
// 
// 		while ( b != NULL ) {
// 			printf( "\t\tBlock Addr 0x%08x\tSize %d\n", (unsigned int)( (char*)b + BLOCK_HEADER_SIZE ), GetBlockSize( b ) );
// 			b = b->pNextFree;
// 		}
// 	}
// }
// 
// void CheckLeakPoint() {
// 	Block* b = (Block*)pStartSentinel + 1;
// 	printf( "block size: %d\n", BLOCK_HEADER_SIZE );
// 
// 	while ( b < pEndSentinel ) {
// 		if ( CheckBlock( b ) && !IsBlockFree( b ) ) {
// #ifdef MEMORY_DEBUG
// 			printf( "Leak point, addr: 0x%08x\tline: %d\tfile %s\n", (unsigned int)(b + BLOCK_HEADER_SIZE), b->line, b->file );
// #else 
// 			printf( "Leak point, addr: 0x%08x\tline: %d\tfile %s\n",(unsigned int)(b + BLOCK_HEADER_SIZE), -1, "" );
// #endif
// 		}
// 		b = b->pNextMem;
// 	}
// 	return;
// }
