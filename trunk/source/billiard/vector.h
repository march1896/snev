#ifndef _VECTOR_H_
#define _VECTOR_H_
class vector2{
public:
	vector2();
	vector2( float _x, float _y );
	vector2( const vector2 &rhs );
	~vector2();

	float *getdata() const;
	vector2 operator+( const vector2 &rhs );
	vector2 operator-( const vector2 &rhs );
	vector2 operator-=( const vector2 &rhs );
	vector2 operator+=( const vector2 &rhs );
	vector2 operator=( const vector2 &rhs );
	bool operator==( const vector2 &rhs );
	float dot( const vector2 &rhs );
	float cross( const vector2 &rhs );
	vector2 normalize();
	float length();

	float getX() const;
	float getY() const;
	void setX( float _x );
	void setY( float _y );
	void setXY( float _x, float _y );
private:
	float x, y;
};

class vector3{
public:
	vector3();
	vector3( float _x, float _y, float _z );
	vector3( const vector3 &rhs );
	~vector3();

	float *getdata() const;
	vector3 operator+( const vector3 &rhs );
	vector3 operator-( const vector3 &rhs );
	vector3 operator+=( const vector3 &rhs );
	vector3 operator-=( const vector3 &rhs );
	vector3 operator=( const vector3 &rhs );
	bool operator==( const vector3 &rhs );
	float dot( const vector3 &rhs );
	vector3 cross( const vector3  &rhs );
	vector3 normalize();
	float length();

	void setXYZ( float _x, float _y, float _z );
	float getX() const;
	float getY() const;
	float getZ() const;
	void setX( float _x );
	void setY( float _y );
	void setZ( float _z );
private:
	float x, y, z;
};

class vector4{
public:
	vector4();
	vector4( float _x, float _y, float _z, float _w );
	vector4( const vector4 &rhs );
	~vector4();

	float *getdata() const;
	void setXYZ( float _x, float _y, float _z );
	void setXYZW( float _x, float _y, float _z, float _w );
	float length();
	float getX() const;
	float getY() const;
	float getZ() const;
	float getW() const;
	void setX( float _x );
	void setY( float _y );
	void setZ( float _z );
	void setW( float _w );

	vector4 operator+( const vector4 &rhs );
	vector4 operator-( const vector4 &rhs );
	vector4 operator+=( const vector4 &rhs );
	vector4 operator-=( const vector4 &rhs );
	vector4 operator=( const vector4 &rhs );
	bool operator==( const vector4 &rhs );
	float dot( const vector4 &rhs );
	vector4 cross( const vector4 &rhs );
	vector4 normalize();
private:
	float x, y, z, w;
};
#endif
