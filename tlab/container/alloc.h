#ifndef _CONTAINER_ALLOC_
#define _CONTAINER_ALLOC_
#include "../heap/heap2.h"
namespace TgLib {
	namespace Container {
		template<typename T>
		struct _alloc_types_ {
			typedef T 	value_type;
			typedef T* 	pointer;
			typedef const T* 	const_pointer;
			typedef T& 	reference;
			typedef const T& 	const_reference;
		};

		template< typename T >
		class Alloc: public _alloc_types_< T > {
		public:
			typedef T 			value_type;
			typedef Pointer 	pointer;
			typedef Reference 	reference;

			/*
			pointer  	allocate() {
				pointer p;
				return p;
			}
			*/
			pointer 	allocate();

			void 		deallocate( pointer p ) {
				return;
			}
		protected:
		};

		template<typename T>
			typename Alloc<T>::pointer 
			Alloc<T>::allocate() {
			}

		/*

		//template< typename T, typename Pointer = T*, typename Reference = T& >
		template< typename T >
		void Alloc<T>::deallocate( T* p ) {
		return; 
		}
		 */
	};
};
#endif //_CONTAINER_ALLOC_
