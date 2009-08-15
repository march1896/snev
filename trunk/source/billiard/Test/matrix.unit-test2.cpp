#include <cstdio>
#include "matrix.h"

void printb( matrix44 *mt )
{
	/*const float *m = mt.getdata();

	for ( int i = 0; i < 4; i ++ )
	{
		for ( int j = 0; j < 4; j ++ )
			printf( "%f\t", m[ 4 * i + j ] );
		printf( "\n" );
	}
	printf( "\n" );
	*/

	printf( "in print func\n" );

	for (int i = 0; i < 4; i ++ ) {
		for ( int j = 0; j < 4; j ++ )
			printf( "%f\t", mt->get( i, j ) );
		printf( "\n" );
	}
	printf( "\n" );
	return;
}

void func()
{
	printf( "fuck the whole world\n\n" );
	return;
}

int main()
{
	matrix44 a, b, c;
	a.translate( 2.0, 3.0, 4.0 );
	b.scale( 2.0, 3.0, 4.0 );

	//matrix44 d( a );
	/*const float * m = a.getdata();

	for ( int i = 0; i < 4; i ++ )
	{
		for ( int j = 0; j < 4; j ++ )
			printf( "%f\t", m[ 4 * i + j ] );
		printf( "\n" );
	}*/
	printf( "been here\n" );

	//c.setdata( b.getdata() );
	c = b;
	printb( &c );
	printf( "printed a\n" );
	printb( &b );

	c = a * b;

	printb( &c );

	int i;
	scanf("%d", &i );
	return 0;
}
