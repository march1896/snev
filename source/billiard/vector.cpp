#include "vector.h"
#include <cmath>

vector2::vector2(): x( 0.0 ), y( 0.0 ) {}
vector2::vector2( float _x, float _y ): x( _x ), y( _y )  {}
vector2::vector2( const vector2 &rhs ): x( rhs.x ), y( rhs.y ) {}
vector2::~vector2(){}

void vector2::setXY( float _x, float _y ){ x = _x, y = _y; }
void vector2::setX( float _x ) { x = _x;	}
void vector2::setY( float _y ) { y = _y;	}
float vector2::getX() {		return x; 	}
float vector2::getY() {		return y; 	}

vector2 vector2::operator=( const vector2 &rhs ) {
	this->x = rhs.x;
	this->y = rhs.y;
	return *this;
}

vector2 vector2::operator+( const vector2 &rhs ) {
	this->x += rhs.x;
	this->y += rhs.y;
	return *this;
}

vector2 vector2::operator-( const vector2 &rhs ) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	return *this;
}

bool vector2::operator==( const vector2 &rhs ) {
	return ( this->x == rhs.x && this->y == rhs.y );
}

float vector2::dot( const vector2 &rhs ) {
	return this->x * rhs.x + this->y * rhs.y;
}

float vector2::cross( const vector2 &rhs ) {
	return this->x * rhs.y - this->y * rhs.x;
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
vector4::vector4( const vector4 &rhs ): x( rhs.x ), y( rhs.y ), z( rhs.z ), w( rhs.w ) {}
vector4::~vector4(){}

void vector4::setXYZ( float _x, float _y, float _z ){ 			x = _x, y = _y, z = _z; 		}
void vector4::setXYZW( float _x, float _y, float _z, float _w ){ 	x = _x, y = _y, z = _z, w = _w; 	}

void vector4::setX( float _x ) { x = _x;	}
void vector4::setY( float _y ) { y = _y;	}
void vector4::setZ( float _z ) { z = _z;	}
void vector4::setW( float _w ) { w = _w;	}
float vector4::getX() {		return x; 	}
float vector4::getY() {		return y; 	}
float vector4::getZ() {		return z; 	}
float vector4::getW() {		return w; 	}

vector4 vector4::operator=( const vector4 &rhs ) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->w = rhs.w;
	return *this;
}

vector4 vector4::operator+( const vector4 &rhs ) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

vector4 vector4::operator-( const vector4 &rhs ) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

bool vector4::operator==( const vector4 &rhs ) {
	return ( this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w );
}

float vector4::dot( const vector4 &rhs ) {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w;
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
vector3::vector3( const vector3 &rhs ): x( rhs.x ), y( rhs.y ), z( rhs.z ) {}
vector3::~vector3(){}

void vector3::setXYZ( float _x, float _y, float _z ){ x = _x, y = _y, z = _z; }
void vector3::setX( float _x ) { x = _x;	}
void vector3::setY( float _y ) { y = _y;	}
void vector3::setZ( float _z ) { z = _z;	}
float vector3::getX() {		return x; 	}
float vector3::getY() {		return y; 	}
float vector3::getZ() {		return z; 	}

vector3 vector3::operator=( const vector3 &rhs ) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	return *this;
}

vector3 vector3::operator+( const vector3 &rhs ) {
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}

vector3 vector3::operator-( const vector3 &rhs ) {
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

bool vector3::operator==( const vector3 &rhs ) {
	return ( this->x == rhs.x && this->y == rhs.y && this->z == rhs.z );
}

float vector3::dot( const vector3 &rhs ) {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

vector3 vector3::cross( const vector3 &rhs ) {
	vector3 ret;
	ret.x = this->y * rhs.z - this->z * rhs.y;
	ret.y = this->x * rhs.z - this->z * rhs.x;
	ret.z = this->x * rhs.y - this->y * rhs.x;
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

