#ifndef _TJ_HEAP2_
#define _TJ_HEAP2_
#include <cstddef>

#define _MEM_DEBUG_
#define _REDIRECT_NEW_

typedef unsigned int uint;

void 	InitHeap(void *Buffer, size_t Size, uint Alignment = 32);
void 	DeinitHeap();

void* 	AllocateHighClean(size_t size);
void* 	AllocateLowClean(size_t size);
void* 	ReAllocateClean(void* Mem, size_t size);
void 	FreeClean(void* ToFree);

#ifdef _MEM_DEBUG_
void* 	AllocateHighDebug(size_t size, uint line, const char* filename);
void* 	AllocateLowDebug(size_t size, uint line, const char* filename);
void* 	ReAllocateDebug(void* Mem, size_t size, uint line, const char* filename);
void 	FreeDebug(void* ToFree, uint line, const char* filename);
#endif

size_t 	GetLargestFree();
size_t 	GetHeapSize();
size_t 	GetMemoryBlockSize( void* Mem );
void 	DumpFreeList();
void 	CheckLeakPoint();

#endif // _TJ_HEAP2_
