#include "color.h"

//This variable is use to return the color as a array;
float color::dummydata[4];
static float tolegal( float para ) {
	if ( para > 1.0 ) return 1.0f;
	else if ( para < 0.0 ) return 0.0f;
	else return para;
}

color::color(): r( 0.0 ), g( 0.0 ), b( 0.0 ), a( 0.0 ) {}
color::color( float _r, float _g, float _b, float _a ): r( _r ), g( _g ), b( _b ), a( _a ) { check(); }
color::color( float _r, float _g, float _b ): r( _r ), g( _g ), b( _b ), a( 1.0f ) { check(); }
color::color( const color &rhs ): r( rhs.getR() ), g( rhs.getG() ), b( rhs.getB() ), a( rhs.getA() ) { check(); }
color::~color(){}

float* color::getdata() const {
	dummydata[ 0 ] = r;
	dummydata[ 1 ] = g;
	dummydata[ 2 ] = b;
	dummydata[ 3 ] = a;
	return dummydata;
}

void color::setRGB( float _r, float _g, float _b ){ 		r = _r, g = _g, b = _b; check();	}
void color::setRGBA( float _r, float _g, float _b, float _a ){ 	r = _r, g = _g, b = _b, a = _a; check();	}

void color::setR( float _r ) { r = tolegal( _r );	}
void color::setG( float _g ) { g = tolegal( _g );	}
void color::setB( float _b ) { b = tolegal( _b );	}
void color::setA( float _a ) { a = tolegal( _a );	}
// we can only return the float value
float color::getR()const {		return r; 	}
float color::getG()const {		return g; 	}
float color::getB()const {		return b; 	}
float color::getA()const {		return a; 	}


void color::check() {
	if ( this->r < 0.0f ) this->r = 0.0f;
	if ( this->r > 1.0f ) this->r = 1.0f;
	if ( this->g < 0.0f ) this->g = 0.0f;
	if ( this->g > 1.0f ) this->g = 1.0f;
	if ( this->b < 0.0f ) this->b = 0.0f;
	if ( this->b > 1.0f ) this->b = 1.0f;
	if ( this->a < 0.0f ) this->a = 0.0f;
	if ( this->a > 1.0f ) this->a = 1.0f;
}

color color::operator=( const color &rhs ) {
	this->r = rhs.getR();
	this->g = rhs.getG();
	this->b = rhs.getB();
	this->a = rhs.getA();
	return *this;
}

color color::operator+=( const color &rhs ) {
	this->r += rhs.getR();
	this->g += rhs.getG();
	this->b += rhs.getB();
	this->a += rhs.getA();
	check();
	return *this;
}

color color::operator-=( const color &rhs ) {
	this->r -= rhs.getR();
	this->g -= rhs.getG();
	this->b -= rhs.getB();
	this->a -= rhs.getA();
	check();
	return *this;
}

color color::operator+( const color &rhs ) {
	return color( this->r + rhs.getR(), this->g + rhs.getG(), this->b + rhs.getB(), this->a );
}

color color::operator-( const color &rhs ) {
	return color( this->r - rhs.getR(), this->g - rhs.getG(), this->b - rhs.getB(), this->a );
}

bool color::operator==( const color &rhs ) {
	return ( this->r == rhs.getR() && this->g == rhs.getG() && this->b == rhs.getB() && this->a == rhs.getA() );
}
