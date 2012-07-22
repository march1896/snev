#ifndef _OO_HEAP_H_
#define _OO_HEAP_H_

#include <new>

namespace Core { namespace Memory {

	class HeapOP {
		public:
			enum EHeapAllocType {
				E_HEAP_SYSTEM,
				E_HEAP_BUDDY,
				E_HEAP_COUNT
			};

		public:
			void* operator new(std::size_t) 		throw(std::bad_alloc); 
			void  operator delete(void*) 			throw(); 
			void* operator new[](std::size_t) 		throw(std::bad_alloc); 
			void  operator delete[](void*) 			throw(); 

			/*
			void* operator new(std::size_t, void* p) 	throw(std::bad_alloc); 
			void  operator delete(void* p, void*) 		throw(); 
			void* operator new[](std::size_t, void* p) 	throw(std::bad_alloc); 
			void  operator delete[](void* p, void*) 	throw(); 
			*/

			static void Init(EHeapAllocType allocType, int size);
			static void Deinit();
	};
}
};
#endif // _OO_HEAP_H_
