#ifndef _OO_HEAP_H_
#define _OO_HEAP_H_

#include <heap_interface.h>

#include <new>

#pragma push_macro("new")
#pragma push_macro("delete")

#undef new
#undef delete

namespace Core { 
namespace Memory {

	class HeapOP {
		public:
			enum EHeapAllocType {
				E_HEAP_SYSTEM,
				E_HEAP_BUDDY,
				E_HEAP_COUNT
			};

		public:

			static void* operator new(std::size_t); 
			static void  operator delete(void*); 
			static void* operator new[](std::size_t); 
			static void  operator delete[](void*); 

			static void* operator new(std::size_t, const char*, std::size_t); 
			static void  operator delete(void*, const char*, std::size_t); 
			static void* operator new[](std::size_t, const char*, std::size_t); 
			static void  operator delete[](void*, const char*, std::size_t); 

			//not allow to use replacement new currently
			static void* operator new(std::size_t sizeInBytes, void* p)			{return p;}
			static void* operator new[](std::size_t sizeInBytes, void* p)		{return p;}
			static void  operator delete(void *, void*)							{}
			static void	 operator delete[](void *, void*)						{}

			static void Init(EHeapAllocType allocType, int size);
			static void Deinit();
	};
}
};

#pragma pop_macro("delete")
#pragma pop_macro("new")

#ifdef MEMORY_DEBUG
#undef new
#define new new(__FILE__, __LINE__)
// #undef delete
// #define delete DeleteTracker(__FILE__, __LINE__), delete
#endif // MEM_DEBUG

#endif // _OO_HEAP_H_
