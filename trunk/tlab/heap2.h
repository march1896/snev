#ifndef _TJ_HEAP2_
#define _TJ_HEAP2_
#include <cstddef>

// #define _MEM_DEBUG_

typedef unsigned int uint;

void 	InitHeap( void *Buffer, size_t Size, uint Alignment = 32 );
void 	DeinitHeap();

void* 	AllocateHighDebug( size_t size, uint line, const char* filename );
void* 	AllocateLowDebug( size_t size, uint line, const char* filename );
void* 	ReAllocateDebug( void* Mem, size_t size, uint line, const char* filename );
void 	FreeDebug( void* ToFree, uint line, const char* filename );

void* 	AllocateHigh( size_t size );
void* 	AllocateLow( size_t size );
void* 	ReAllocate( void* Mem, size_t size );
void 	Free( void* ToFree );

size_t 	GetLargestFree();
size_t 	GetHeapSize();
size_t 	GetMemoryBlockSize( void* Mem );
void 	DumpFreeList();

#ifdef _MEM_DEBUG_
#define AllocateHigh( size ) AllocateHighDebug( size, __LINE__, __FILE__ )
#define AllocateLow( size ) AllocateLowDebug( size, __LINE__, __FILE__ )
#define Allocate( size ) AllocateLowDebug( size, __LINE__, __FILE__ )
#define ReAllocate( Mem, size ) ReAllocateDebug( Mem, size, __LINE__, __FILE__ )
#define Free( ToFree ) FreeDebug( ToFree )
#else // Final
#define Allocate( size ) AllocateLow( size )
#endif // _MEM_DEBUG_

#endif // _TJ_HEAP2_
