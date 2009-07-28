#ifndef _VECTOR_H_
#define _VECTOR_H_
class vector4{
public:
	vector4();
	vector4( float _x, float _y, float _z, float _w );
	vector4( const vector4 &rhs );
	void setXYZ( float _x, float _y, float _z );
	void setXYZW( float _x, float _y, float _z, float _w );
	vector4 operator+( const vector4 &rhs );
	vector4 operator-( const vector4 &rhs );
	vector4 operator=( const vector4 &rhs );
	bool operator==( const vector4 &rhs );
	float getX();
	float getY();
	float getZ();
	float getW();
	void setX( float _x );
	void setY( float _y );
	void setZ( float _z );
	void setW( float _w );
	~vector4();
private:
	float x, y, z, w;
};

class vector3{
public:
	vector3();
	vector3( float _x, float _y, float _z );
	vector3( const vector3 &rhs );
	void setXYZ( float _x, float _y, float _z );
	vector3 operator+( const vector3 &rhs );
	vector3 operator-( const vector3 &rhs );
	vector3 operator=( const vector3 &rhs );
	bool operator==( const vector3 &rhs );
	float getX();
	float getY();
	float getZ();
	void setX( float _x );
	void setY( float _y );
	void setZ( float _z );
	~vector3();
private:
	float x, y, z;
};

#endif
