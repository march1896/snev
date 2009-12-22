#ifndef _CONTAINER_LIST_
#define _CONTAINER_LIST_
#include "alloc.h"
namespace TgLib {
	namespace Container {

	template< typename T, typename Allocator = Alloc >
	class List {
		public:
			typedef T 	Type;
			typedef T* 	Pointer;
			typedef T& 	Reference;
		public:
			List();
			List( const List& _l );
			~List();

			
		protected:
			Pointer 	Data;
	};

	};
};
#endif //_CONTAINER_LIST_
