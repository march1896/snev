#ifndef _OO_HEAP_H_
#define _OO_HEAP_H_
#include <heap_interface.h>

#include <new>

#ifdef MEMORY_DEBUG
#define HEAPOP_DEBUG_MODE
#endif

namespace Core { namespace Memory {

	class HeapOP {
		public:
			enum EHeapAllocType {
				E_HEAP_SYSTEM,
				E_HEAP_BUDDY,
				E_HEAP_COUNT
			};

		public:
			static void* operator new(std::size_t) 		throw(std::bad_alloc); 
			static void  operator delete(void*) 			throw(); 
			static void* operator new[](std::size_t) 		throw(std::bad_alloc); 
			static void  operator delete[](void*) 			throw(); 

#ifdef HEAPOP_DEBUG_MODE
			static void* operator new(std::size_t, const char*, std::size_t) 		throw(std::bad_alloc); 
			static void  operator delete(void*, const char*, std::size_t) 			throw(); 
			static void* operator new[](std::size_t, const char*, std::size_t) 		throw(std::bad_alloc); 
			static void  operator delete[](void*, const char*, std::size_t) 		throw(); 
#endif
			/*
			 * not allow to use replacement new currently
			 *
			void* operator new(std::size_t, void* p) 	throw(std::bad_alloc); 
			void  operator delete(void* p, void*) 		throw(); 
			void* operator new[](std::size_t, void* p) 	throw(std::bad_alloc); 
			void  operator delete[](void* p, void*) 	throw(); 
			*/

			static void Init(EHeapAllocType allocType, int size);
			static void Deinit();
	};
#ifdef HEAPOP_DEBUG_MODE
#undef new
#define new new(__FILE__, __LINE__)
#endif // MEM_DEBUG
}
};
#endif // _OO_HEAP_H_
