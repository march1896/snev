#include <iostream>
#include "color.h"
using namespace std;

void print( float a )
{
	cout << " I am float" << endl;
}

void print( int a ) 
{
	cout << " I am int " << endl;
}

class object {
public:
	explicit object( unsigned a ) {
		this->a = a;
		cout << "I am int" << endl;
	}
	object( float a, float b ) { 
		this->a = a;
		cout << "I am float" << endl;
	}
private:
	float a, b, c, d;
};

int main()
{
	color red( 1.0, 0.0, 0.0, 0.0 );
	int i;
	float f;
	object ii( i );
	object ff( f );
	print( i );
	print( f );
	cin.get();
	return 0;
}

