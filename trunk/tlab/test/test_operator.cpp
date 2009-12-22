#include <iostream>
using namespace std;

class Number {
public:
	Number(): data( 0 ) {}
	Number( int i ): data( i ) {}
	const Number& operator+=( const Number& num );
private:
	int data;
};

const Number& Number::operator+=( const Number& num ) {
	data += num.data;
	return *this;
}

int main() {
	Number a( 5 );
	Number b( 7 );

	Number* c = new Number( 15 );
	Number* d = new Number( 27 );
	a += b;
	a += *d;
	c->operator+=( b );
	cout << a.data << endl;
	cin.get();
	return 0;
}
