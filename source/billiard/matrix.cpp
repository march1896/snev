#include "matrix.h"
#include "math.h"
#include <cassert>
#define IDX(x,y) (4*(x)+(y))

matrix44::matrix44() {
	this->identity();
}

matrix44::matrix44( const float *mp ) {
	assert( mp );
	for (int i = 0; i < 16; i++ )
		this->data.m[ i ] = mp[ i ];
}

matrix44::matrix44( const matrix44 &rhs ) {
	*this = rhs;
}

matrix44::~matrix44() {}

void matrix44::zero()
{
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			this->data.m[ IDX( i, j ) ] = 0.0f;
}

void matrix44::identity()
{
	this->zero();
	for ( int i = 0; i < 4; i++ )
		this->data.m[ IDX( i, i ) ] = 1.0f;
}

matrix44& matrix44::operator=( const matrix44 &rhs ) {
	this->setdata( rhs.getdata() );
	return *this;
}

const float* matrix44::getdata() const 
{
	return this->data.m;
}

void matrix44::setdata( const float* _data )
{
	for ( int i = 0; i < 16; i++ )
		this->data.m[i] = _data[i];
}

void matrix44::set( int x, int y, float value )
{
	assert( x >= 0 && x < 4 );
	assert( y >= 0 && y < 4 );
	this->data.m[ IDX(x, y) ] = value;
}

float matrix44::get( int x, int y )
{
	assert( x >= 0 && x < 4 );
	assert( y >= 0 && y < 4 );
	return this->data.m[ IDX(x, y) ];
}


matrix44 operator*( const matrix44 &lhs, const matrix44 &rhs ) {
	matrix44 ret;
	float temp = 0.0f;
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			temp = 0.0f;
			for ( int k = 0; k < 4; k++ )
				temp += lhs.data.m[ IDX( i, k )] * rhs.data.m[ IDX( k, j ) ];
			ret.data.m[ IDX( i, j ) ] = temp;
		}
	return ret;
}

bool matrix44::operator==( const matrix44 &rhs ) {
	const float *m = rhs.getdata();
	for ( int i = 0; i < 16; i++ )
		if ( this->data.m[i] != m[i] )
			return false;
	return true;
}
	

void matrix44::translate(  float x,  float y,  float z ) {
	data.tx += data.r00*x + data.r10*y + data.r20*z;
	data.ty += data.r01*x + data.r11*y + data.r21*z;
	data.tz += data.r02*x + data.r12*y + data.r22*z;
}

void matrix44::scale( float x, float y, float z )
{
	data.r00 *= x;
	data.r11 *= y;
	data.r22 *= z;
}


/* pre multiply 
 * [ 1     0      0       0
 *   0   cos(a)  sin(a)   0
 *   0  -sin(a)  cos(a)   0
 *   0     0      0       1  ]
 */
void matrix44::rotate_x( float angle )
{
	float sx = sin( angle ), cx = cos( angle );
	matrix44 temp;
	temp.identity();

	temp.set( 1, 1, cx );
	temp.set( 2, 2, cx );
	temp.set( 1, 2, sx );
	temp.set( 2, 1, -sx );

	this->left_mult( temp );
}

void matrix44::rotate_y( float angle )
{
	float sx = sin( angle ), cx = cos( angle );
	matrix44 temp;
	temp.identity();

	temp.set( 0, 0, cx );
	temp.set( 2, 2, cx );
	temp.set( 0, 2, -sx );
	temp.set( 2, 0, sx );

	this->left_mult( temp );
}

void matrix44::rotate_z( float angle )
{
	float sx = sin( angle ), cx = cos( angle );
	matrix44 temp;
	temp.identity();

	temp.set( 0, 0, cx );
	temp.set( 1, 1, cx );
	temp.set( 0, 1, sx );
	temp.set( 1, 0, -sx );

	this->left_mult( temp );
}

void matrix44::rotate_xyz(  float x,  float y,  float z,  float angle )
{
	float alpha, beta, xz, xyz, s_alpha, c_alpha, s_beta, c_beta;
	xz = sqrt( x*x + z*z );
	if ( xz < 10e-10 ) {
		s_alpha = 0.0f;
		c_alpha = 1.0f;
	}else {
		s_alpha = z / xz;
		c_alpha = x / xz;
	}
	xyz = sqrt( x*x + y*y + z*z );
	s_beta = y / xyz;
	c_beta = xz / xyz;
	if ( x < 0.0f ) c_beta = -c_beta;

	matrix44 temp;

	float sx, cx;
	sx = sin( angle ), cx = cos( angle );
	temp.identity();

	temp.set( 0, 0, c_alpha );
	temp.set( 2, 2, c_alpha );
	temp.set( 0, 2, s_alpha );
	temp.set( 2, 0, -s_alpha );
	this->left_mult( temp );

	temp.identity();
	temp.set( 0, 0, c_beta );
	temp.set( 1, 1, c_beta );
	temp.set( 0, 1, s_beta );
	temp.set( 1, 0, -s_beta );
	this->left_mult( temp );

	temp.identity();
	temp.set( 1, 1, cx );
	temp.set( 2, 2, cx );
	temp.set( 1, 2, sx );
	temp.set( 2, 1, -sx );
	this->left_mult( temp );

	temp.identity();
	temp.set( 0, 0, c_beta );
	temp.set( 1, 1, c_beta );
	temp.set( 0, 1, -s_beta );
	temp.set( 1, 0, s_beta );
	this->left_mult( temp );

	temp.identity();
	temp.set( 0, 0, c_alpha );
	temp.set( 2, 2, c_alpha );
	temp.set( 0, 2, -s_alpha );
	temp.set( 2, 0, s_alpha );
	this->left_mult( temp );
}

/*static matrix * matrix_rotate_xyz( matrix *a, float x, float y, float z, float angle )  
{
	// first rotate (x, y, z ) to ( 1, 0, 0 ), then rotate_x angle
	// use this method will cause five matrix operations
	float alpha, beta, xz, xyz, s_alpha, c_alpha, s_beta, c_beta;
	xz = sqrt( x*x + z*z );
	if ( xz < 10e-10 ) {
		s_alpha = 0.0f;
		c_alpha = 1.0f;
	}else {
		s_alpha = z / xz;
		c_alpha = x / xz;
	}
	xyz = sqrt( x*x + y*y + z*z );
	s_beta = y / xyz;
	c_beta = xz / xyz;
	if ( x < 0.0f ) c_beta = -c_beta;

	matrix temp;

	float sx, cx;
	sx = sin( angle ), cx = cos( angle );

	matrix_identity( &temp );
	temp.r00 = temp.r22 = c_alpha;
	temp.r02 = s_alpha;
	temp.r20 = -s_alpha;
	matrix_left_mult( a, &temp );

	matrix_identity( &temp );
	temp.r00 = temp.r11 = c_beta;
	temp.r01 = s_beta;
	temp.r10 = -s_beta;
	matrix_left_mult( a, &temp );

	matrix_identity( &temp );
	temp.r11 = temp.r22 = cx;
	temp.r12 = sx;
	temp.r21 = -sx;
	matrix_left_mult( a, &temp );

	matrix_identity( &temp );
	temp.r00 = temp.r11 = c_beta;
	temp.r01 = -s_beta;
	temp.r10 = s_beta;
	matrix_left_mult( a, &temp );

	matrix_identity( &temp );
	temp.r00 = temp.r22 = c_alpha;
	temp.r02 = -s_alpha;
	temp.r20 = s_alpha;
	matrix_left_mult( a, &temp );

	return a;
}
*/


// this = rhs * this;
matrix44 matrix44::left_mult( const matrix44 &rhs ) 
{
	matrix44 temp;

	temp = rhs * (*this);

	*this = temp;

	return *this;
}

matrix44 matrix44::right_mult( const matrix44 &rhs )
{
	matrix44 temp;
	temp = *this * rhs;
	
	*this = temp;
	return *this;
}
