#ifndef _TJ_HEAP2_
#define _TJ_HEAP2_
#include <cstddef>

#define _MEM_DEBUG_
#define _REDIRECT_NEW_

typedef unsigned int uint;

void 	InitHeap( void *Buffer, size_t Size, uint Alignment = 32 );
void 	DeinitHeap();

void* 	AllocateHighClean( size_t size );
void* 	AllocateLowClean( size_t size );
void* 	ReAllocateClean( void* Mem, size_t size );
void 	FreeClean( void* ToFree );

#ifdef _MEM_DEBUG_
void* 	AllocateHighDebug( size_t size, uint line, const char* filename );
void* 	AllocateLowDebug( size_t size, uint line, const char* filename );
void* 	ReAllocateDebug( void* Mem, size_t size, uint line, const char* filename );
void 	FreeDebug( void* ToFree, uint line, const char* filename );
#endif

size_t 	GetLargestFree();
size_t 	GetHeapSize();
size_t 	GetMemoryBlockSize( void* Mem );
void 	DumpFreeList();
void 	CheckLeakPoint();

#ifdef _MEM_DEBUG_
#define Allocate( size ) AllocateLowDebug( size, __LINE__, __FILE__ )
#define ReAllocate( Mem, size ) ReAllocateDebug( Mem, size, __LINE__, __FILE__ )
#define Free( ToFree ) FreeDebug( ToFree, __LINE__, __FILE__ )
#else // Final
#define Allocate( size ) AllocateLowClean( size )
#define ReAllocate( Mem, size ) ReAllocateClean( Mem, size )
#define Free( ToFree ) FreeClean( ToFree )
#endif // _MEM_DEBUG_

#ifdef _REDIRECT_NEW_
#ifdef _MEM_DEBUG_
inline void* operator new 		( size_t Size, uint p1, uint p2, uint Line, const char* FileName ) throw() 	{ return AllocateLowDebug( Size, Line, FileName ); }
inline void* operator new[] 	( size_t Size, uint p1, uint p2, uint Line, const char* FileName ) throw()	{ return AllocateLowDebug( Size, Line, FileName ); }
//inline void  operator delete 	( void* Mem,   size_t size, uint p2, uint Line, const char* FileName ) 	{ return FreeDebug( Mem, Line, FileName ); }
//inline void  operator delete[] 	( void* Mem,   size_t size, uint p2, uint Line, const char* FileName ) 	{ return FreeDebug( Mem, Line, FileName ); }
inline void 	operator delete( void* Mem ) throw()			{ FreeClean( Mem ); }
inline void 	operator delete[]( void* Mem ) throw()			{ FreeClean( Mem ); } 
//#define new new( 0, 0, __LINE__, __FILE__ )
//#define delete delete( 0, __LINE__, __FILE__ )

#else // !_MEM_DEBUG_
inline void* 	operator new( size_t Size ) throw()				{ return AllocateClean( Size ); }
inline void* 	operator new[]( size_t Size ) throw() 			{ return AllocateClean( Size ); }
inline void 	operator delete( void* Mem ) throw() 			{ FreeClean( Mem ); }
inline void 	operator delete[]( void* Mem ) throw() 			{ FreeClean( Mem ); } 
#endif // _MEM_DEBUG_
#endif //_REDIRECT_NEW_

#endif // _TJ_HEAP2_
