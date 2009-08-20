#include "vector.h"
#include <cmath>

static float dummy[ 4 ];

vector2::vector2(): x( 0.0 ), y( 0.0 ) {}
vector2::vector2( float _x, float _y ): x( _x ), y( _y )  {}
vector2::vector2( const vector2 &rhs ): x( rhs.getX() ), y( rhs.getY() ) {}
vector2::~vector2(){}

void vector2::setXY( float _x, float _y ){ x = _x, y = _y; }
void vector2::setX( float _x ) { x = _x;	}
void vector2::setY( float _y ) { y = _y;	}
float vector2::getX()const {		return x; 	}
float vector2::getY()const {		return y; 	}

float* vector2::getdata() const {
	dummy[0] = x;
	dummy[1] = y;
	return dummy;
}

vector2 vector2::scale( float para ) {
	this->x *= para;
	this->y *= para;
	return *this;
}
vector2 vector2::operator=( const vector2 &rhs ) {
	this->x = rhs.getX();
	this->y = rhs.getY();
	return *this;
}

vector2 vector2::operator+=( const vector2 &rhs ) {
	this->x += rhs.getX();
	this->y += rhs.getY();
	return *this;
}

vector2 vector2::operator-=( const vector2 &rhs ) {
	this->x -= rhs.getX();
	this->y -= rhs.getY();
	return *this;
}

vector2 vector2::operator+( const vector2 &rhs ) {
	return vector2( this->x + rhs.getX(), this->y + rhs.getY() );
}

vector2 vector2::operator-( const vector2 &rhs ) {
	return vector2( this->x - rhs.getX(), this->y - rhs.getY() );
}

bool vector2::operator==( const vector2 &rhs ) {
	return ( this->x == rhs.getX() && this->y == rhs.getY() );
}

float vector2::dot( const vector2 &rhs ) {
	return this->x * rhs.getX() + this->y * rhs.getY();
}

float vector2::cross( const vector2 &rhs ) {
	return this->x * rhs.getY() - this->y * rhs.getX();
}

float vector2::length() {
	return sqrt( this->dot( *this ) );
}

vector2 vector2::normalize() {
	float length = this->length();
	this->x /= length;
	this->y /= length;
	return *this;
}

vector4::vector4(): x( 0.0 ), y( 0.0 ), z( 0.0 ), w( 0.0 ) {}
vector4::vector4( float _x, float _y, float _z, float _w ): x( _x ), y( _y ), z( _z ), w( _w ) {}
vector4::vector4( const vector4 &rhs ): x( rhs.getX() ), y( rhs.getY() ), z( rhs.getZ() ), w( rhs.getW() ) {}
vector4::~vector4(){}

void vector4::setXYZ( float _x, float _y, float _z ){ 			x = _x, y = _y, z = _z; 		}
void vector4::setXYZW( float _x, float _y, float _z, float _w ){ 	x = _x, y = _y, z = _z, w = _w; 	}

void vector4::setX( float _x ) { x = _x;	}
void vector4::setY( float _y ) { y = _y;	}
void vector4::setZ( float _z ) { z = _z;	}
void vector4::setW( float _w ) { w = _w;	}
float vector4::getX()const {		return x; 	}
float vector4::getY()const {		return y; 	}
float vector4::getZ()const {		return z; 	}
float vector4::getW()const {		return w; 	}
float* vector4::getdata() const {
	dummy[0] = x;
	dummy[1] = y;
	dummy[2] = z;
	dummy[3] = w;
	return dummy;
}

vector4 vector4::scale( float para ) {
	this->x *= para;
	this->y *= para;
	this->z *= para;
	// this->w *= para;
	return *this;
}

vector4 vector4::operator=( const vector4 &rhs ) {
	this->x = rhs.getX();
	this->y = rhs.getY();
	this->z = rhs.getZ();
	this->w = rhs.getW();
	return *this;
}

vector4 vector4::operator+=( const vector4 &rhs ) {
	this->x += rhs.getX();
	this->y += rhs.getY();
	this->z += rhs.getZ();
	return *this;
}

vector4 vector4::operator-=( const vector4 &rhs ) {
	this->x -= rhs.getX();
	this->y -= rhs.getY();
	this->z -= rhs.getZ();
	return *this;
}

vector4 vector4::operator+( const vector4 &rhs ) {
	return vector4( this->x + rhs.getX(), this->y + rhs.getY(), this->z + rhs.getZ(), this->w );
}

vector4 vector4::operator-( const vector4 &rhs ) {
	return vector4( this->x - rhs.getX(), this->y - rhs.getY(), this->z - rhs.getZ(), this->w );
}

bool vector4::operator==( const vector4 &rhs ) {
	return ( this->x == rhs.getX() && this->y == rhs.getY() && this->z == rhs.getZ() && this->w == rhs.getW() );
}

float vector4::dot( const vector4 &rhs ) {
	return this->x * rhs.getX() + this->y * rhs.getY() + this->z * rhs.getZ() + this->w;
}

vector4 vector4::cross( const vector4 &rhs ) {
}

float vector4::length() {
	return sqrt( this->dot( *this ) );
}

vector4 vector4::normalize() {
	float length = this->length();
	this->x /= length;
	this->y /= length;
	this->z /= length;
	return *this;
}




vector3::vector3(): x( 0.0 ), y( 0.0 ), z( 0.0 ) {}
vector3::vector3( float _x, float _y, float _z ): x( _x ), y( _y ), z( _z )  {}
vector3::vector3( const vector3 &rhs ): x( rhs.getX() ), y( rhs.getY() ), z( rhs.getZ() ) {}
vector3::~vector3(){}

void vector3::setXYZ( float _x, float _y, float _z ){ x = _x, y = _y, z = _z; }
void vector3::setX( float _x ) { x = _x;	}
void vector3::setY( float _y ) { y = _y;	}
void vector3::setZ( float _z ) { z = _z;	}
float vector3::getX()const {		return x; 	}
float vector3::getY()const {		return y; 	}
float vector3::getZ()const {		return z; 	}
float* vector3::getdata() const {
	dummy[0] = x;
	dummy[1] = y;
	dummy[2] = z;
	return dummy;
}

vector3 vector3::scale( float para ) {
	this->x *= para;
	this->y *= para;
	this->z *= para;
	return *this;
}
vector3 vector3::operator=( const vector3 &rhs ) {
	this->x = rhs.getX();
	this->y = rhs.getY();
	this->z = rhs.getZ();
	return *this;
}

vector3 vector3::operator+=( const vector3 &rhs ) {
	this->x += rhs.getX();
	this->y += rhs.getY();
	this->z += rhs.getZ();
	return *this;
}

vector3 vector3::operator-=( const vector3 &rhs ) {
	this->x -= rhs.getX();
	this->y -= rhs.getY();
	this->z -= rhs.getZ();
	return *this;
}

vector3 vector3::operator+( const vector3 &rhs ) {
	return vector3( this->x + rhs.getX(), this->y + rhs.getY(), this->z + rhs.getZ() );
}

vector3 vector3::operator-( const vector3 &rhs ) {
	return vector3( this->x - rhs.getX(), this->y - rhs.getY(), this->z - rhs.getZ() );
}

bool vector3::operator==( const vector3 &rhs ) {
	return ( this->x == rhs.getX() && this->y == rhs.getY() && this->z == rhs.getZ() );
}

float vector3::dot( const vector3 &rhs ) {
	return this->x * rhs.getX() + this->y * rhs.getY() + this->z * rhs.getZ();
}

vector3 vector3::cross( const vector3 &rhs ) {
	vector3 ret;
	ret.setX( this->y * rhs.getZ() - this->z * rhs.getY() );
	ret.setY( this->x * rhs.getZ() - this->z * rhs.getX() );
	ret.setZ( this->x * rhs.getY() - this->y * rhs.getX() );
	return ret;
}

float vector3::length() {
	return sqrt( this->dot( *this ) );
}

vector3 vector3::normalize() {
	float length = this->length();
	this->x /= length;
	this->y /= length;
	this->z /= length;
	return *this;
}

