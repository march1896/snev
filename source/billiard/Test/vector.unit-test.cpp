#include "../vector.h"
#include <iostream>
using namespace std;

void vector2_unit_test()
{
#define print( v ) \
	cout << ( v.getX() ) << " " << ( v.getY() ) << endl;
	vector2 a( 1.0, 1.0 );
	vector2 b( a );
	vector2 c( 3.0, 3.0 );

	c = a + b;
	a += c;
	cout << a.length() << endl;
	print( a );
	print( b );
	b -= c;
	print( b );
	a.normalize();
	print( a );
}

int main()
{
	cout << "hello, tangel" << endl;

	vector2_unit_test();
	return 0;
}
