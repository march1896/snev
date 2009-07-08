#include "math.h"
//#include "sincos.h"

#include "stdio.h"

/*    y1
 *   / |
 *  /__|y0
 *  |  |
 *  |__|
 *  x0, x1
 *
 *  delta_y 	= ( y1 - y0 ) / ( x1 - x0 ) 
 *  		= ( y1 - y0 ) / ( 1 << 8 ) 
 *  sin( x ) 	= y0 + ( x - x0 ) * delta_y
 *   		= x * delta_y + y0 - x0 * delta_y
 *   		= x * delta_y + ( y0 - x0 * delta_y ); 	
 */
// radian, 0 ~ radian_PI( 3.14 )
// degree, 0 ~ 360.0
// angle, 0 ~ 1 << 16
typedef unsigned short tangle;
const float float_PI = 3.1415926535897932384626433832795028841972; //!<floating point value of pi

float sinTable[ 256 ][ 2 ];


float degree2radian( float __degree )
{
	return __degree * float_PI / 180.0;
}

static tangle radian2angle( float __angle )
{
	return  tangle ( __angle * ( 1 << 15 ) / float_PI );
}

static tangle degree2angle( float __degree )
{
	return tangle (__degree / 360.0 * ( 1 << 16 ) );
}

float tsin( float __degree )
{
	tangle x = degree2angle( __degree );

	return sinTable[ x >> 8 ][ 0 ] + x * sinTable[ x >> 8 ][ 1 ];
}

// this is the method how vclibrary build the sincos table
static void t_generateTable( void ) 
{
	float x0, x1, y0, y1, delta_y;

	for ( int i = 0; i < 256; i++ )
	{
		x0 = i * float_PI / 128.0; 
		x1 = ( i + 1 ) * float_PI / 128.0;
		y0 = sin( x0 );
		y1 = sin( x1 );
		delta_y = ( y1 - y0 ) / 256.0;

		sinTable[ i ][ 1 ] = delta_y;
		//sinTable[ i ][ 0 ] = y0 - i * ( 1 << 8 ) * delta_y;
		sinTable[ i ][ 0 ] = y0 - i * ( y1 - y0 );
	}
	return;
}

int main()
{
	t_generateTable();
	for ( int i = 0; i < 256; i++ )
	{
		printf( "%e\t%e\n", sinTable[ i ][ 0 ], sinTable[ i ][ 1 ] );
	}

	/*printf("test table\n" );
	for ( int i = -360; i < 360; i++ )
	{
		printf( "%f\t%f\n", tsin( i ), sin( i * float_PI / 180.0 ) );
	}*/
	printf("%f\n", tsin( 30.0 ) );
	return 0;
}
		

