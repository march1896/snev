#include "..\matrix.h"
#include <iostream>
using namespace std;

void print( const matrix44 &mrt )
{
	const float *m = mrt.getdata();
	for ( int i = 0; i < 4; i++ )
	{
		for ( int j = 0; j < 4; j ++ )
			printf("%f\t", m[ 4*i + j ] );
		printf( "\n" );
	}
	printf( "\n" );
}

int main()
{
	matrix44 m1, m2;

	m1.scale( 3.0, 4.0, 5.0 );	
	print( m1 );

	m1.translate( 1.0, 1.0, 1.0 );
	print( m1 );

	const float *fa = m1.getdata();
	for ( int i = 0; i < 4; i ++ ) {
		for ( int j = 0; j < 4; j ++ )
			printf( "%f\t", fa[4*i+j] );
		printf( "\n");
	}
	printf("\n");


	//m2 = m1;
	m2.setdata( fa );
	matrix44 m3( fa );
	print( m2 );

	//m1.rotate_x( 30.0 );
	m3 = m1 * m2;
	print( m3 );

	int a;
	scanf("%d", &a);
	return 0;
}
