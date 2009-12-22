#include "matrix.h"
#include "math.h"
#define IDX(x,y) 4*(x)+(y)

matrix * matrix_translate( matrix *a, float x, float y, float z )
{
	a->tx += a->r00*x + a->r10*y + a->r20*z;
	a->ty += a->r01*x + a->r11*y + a->r21*z;
	a->tz += a->r02*x + a->r12*y + a->r22*z;
	return a;
}

matrix * matrix_scale( matrix *a, float x, float y, float z )
{
	a->r00 *= x;
	a->r11 *= y;
	a->r22 *= z;
	return a;
}

matrix * matrix_set_value( matrix *a, int x, int y, float value )
{
	//assert( x >= 0 && x < 4 );
	//assert( y >= 0 && y < 4 );
	a->m[IDX(x,y)] = value;
	return a;
}

matrix * matrix_get_value( matrix *a, int x, int y, float &value )
{	
	//assert( x >= 0 && x < 4 );
	//assert( y >= 0 && y < 4 );
	value = a->m[IDX(x,y)];
	return a;
}

/* pre multiply 
 * [ 1     0      0       0
 *   0   cos(a)  sin(a)   0
 *   0  -sin(a)  cos(a)   0
 *   0     0      0       1  ]
 */
matrix * matrix_rotate_x( matrix *a, float angle )
{
	float sx = sin( angle ), cx = cos( angle );
	matrix temp;
	matrix_identity( &temp );

	//matrix_set_value( &temp, 1, 1, cx );
	//matrix_set_value( &temp, 1, 2, sx );
	//matrix_set_value( &temp, 2, 1, -sx );
	//matrix_set_value( &temp, 2, 2, cx );
	temp.r11 = temp.r22 = cx;
	temp.r12 = sx;
	temp.r21 = -sx;

	// a = temp*a;
	matrix_left_mult( a, &temp );
	return a;
}

matrix * matrix_rotate_y( matrix *a, float angle )
{
	float sx = sin( angle ), cx = cos( angle );
	matrix temp;
	matrix_identity( &temp );

	temp.r00 = temp.r22 = cx;
	temp.r02 = -sx;
	temp.r20 = sx;

	// a = temp*a;
	matrix_left_mult( a, &temp );
	return a;
}

matrix * matrix_rotate_z( matrix *a, float angle )
{
	float sx = sin( angle ), cx = cos( angle );
	matrix temp;
	matrix_identity( &temp );

	temp.r00 = temp.r11 = cx;
	temp.r01 = sx;
	temp.r10 = -sx;

	// a = temp*a;
	matrix_left_mult( a, &temp );
	return a;
}

matrix * matrix_rotate_xyz( matrix *a, float x, float y, float z, float angle )  
{
	// first rotate (x, y, z ) to ( 1, 0, 0 ), then rotate_x angle
	// use this method will casue five matrix operations
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


matrix * matrix_identity( matrix* a )
{
	//memset( a, 0, sizeof( a ) );
	for (int i=0; i<16; i++)
		a->m[i] = 0.0f;
	a->r00 = a->r11 = a->r22 = a->m33 = 1.0f;
	return a;
}

matrix * matrix_zero( matrix* a )
{
	for (int i=0; i<16; i++)
		a->m[i] = 0.0f;
	return a;
}

matrix * matrix_copy( matrix *a, matrix *b ) // a = b
{
	for (int i=0; i<16; i++) 
		a->m[i] = b->m[i];
	return a;
}

matrix * matrix_multiply( matrix *a, matrix *b, matrix *v )  //  v = a * b;
{
	/*  stupid method
	matrix temp = *a;
	temp->r00 = a->r00*b->r00 + a->r01*b->r10 + a->r02*b->r20 + a->m03*b->tx;
	temp->r01 = a->r00*b->r01 + a->r01*b->r11 + a->r02*b->r21 + a->m03*b->ty;
	temp->r02 = a->r00*b->r02 + a->r01*b->r12 + a->r02*b->r22 + a->m03*b->tz;
	temp->m03 = a->r00*b->m03 + a->r01*b->m13 + a->r02*b->m23 + a->m03*b->m33;

	temp->r10 = a->r10*b->r00 + a->r11*b->r10 + a->r12*b->r20 + a->m13*b->tx;
	temp->r11 = a->r10*b->r01 + a->r11*b->r11 + a->r12*b->r21 + a->m13*b->ty;
	temp->r12 = a->r10*b->r02 + a->r11*b->r12 + a->r12*b->r22 + a->m13*b->tz;
	temp->m13 = a->r10*b->r00 + a->r11*b->r10 + a->r12*b->r20 + a->m13*b->m33;

	temp->r20 = a->r20*b->r00 + a->r21*b->r10 + a->r22*b->r20 + a->m23*b->tx;
	temp->r21 = a->r20*b->r01 + a->r21*b->r11 + a->r22*b->r21 + a->m23*b->ty;
	temp->r22 = a->r20*b->r02 + a->r21*b->r12 + a->r22*b->r22 + a->m23*b->tz;
	temp->m23 = a->r20*b->m03 + a->r21*b->m13 + a->r22*b->m23 + a->m23*b->m33;
	*/
	matrix_zero( v );

	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			for (int k=0; k<4; k++)
				v->m[IDX(i, j)] += a->m[IDX(i,k)] * b->m[IDX(k,j)];
	return v;
}

matrix * matrix_right_mult( matrix * a, matrix *b ) // a = a*b;
{
	matrix temp;

	matrix_multiply( a, b, &temp );

	matrix_copy( a, &temp );

	return a;
}

matrix * matrix_left_mult( matrix *a, matrix *b ) // a = b*a;
{
	matrix temp;

	matrix_multiply( b, a, &temp );

	matrix_copy( a, &temp );
	return a;
}

matrix * matrix_add( matrix *a, matrix *b )  // a += b;
{
	for (int i=0; i<16; i++)
		a->m[i] += b->m[i];
	return a;
}
