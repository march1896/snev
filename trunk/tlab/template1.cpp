// try template traits

#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------------
template< typename T >
struct SS {
	T 	data;
	T 	GetData() 	{ 	return data; 	};
	void SetData( T a ) 	{ 	data = a; 	};
};

template< typename T >
struct SS< T* > {
	T* 	pointer;
	T* 	GetPointer() 	{ 	return pointer; 	};
	T 	GetData() 		{ 	return *pointer; 	};
	T* 	SetPointer( T* p ) 	{ 	pointer = p; 	};
};

//------------------------------------------------------------------------------------
class A {
public:
	typedef int U32;
	static 	U32 data;
};
A::U32 A::data = 100;

template< typename _type >
class B {
public:
	B() {
		typename _type::U32 *p;
		// _type::U32 *p;    oops, the compiler complained, variable 'p' was not declared yet.
	}
};

//------------------------------------------------------------------------------------
class _a {public: typedef int type; };
class _b:public _a {public: type data; };

template< typename type >
class _type_base {
public:
	typedef type 	Type;
	typedef type* 	Pointer;
	typedef type& 	Refernce;
};

template< typename type >
class Special: public _type_base< type >
{
public:
	typename _type_base<type>::Type data1;
	typename _type_base<type>::Pointer data2;
};

int main() {
	// first example, shows the specialization of template class SS
	SS< int > s;
	SS< int*> t;
	int f = 77;
	s.SetData( 5 );
	//s.SetPointer( &f );    it's wrong
	t.SetPointer( &f );

	// second example, shows the condition we have to use keyword 'typename'
	B< A > b;
	//typename A::data;

	// third example
	Special< int > si;
	si.data1 = 10;
	si.data2 = &f;

	printf( "%d, %d\n", s.GetData(), t.GetData() );

	return 0;
}
