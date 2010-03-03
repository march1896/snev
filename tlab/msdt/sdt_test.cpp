#include "sdt_def.h"
#include "sdt_memory.h"
#include "sdt_pipe.h"
#include "msdt.h"
#include <cstdio>

#define N_NUMBER 43560
#define N_PERLINE 70

int main(int argc, char* argv[])
{
	printf( "size of float %d\n", sizeof( float ) );
	printf( "size of frame %d\n", sizeof( sdt_frame ) );
	printf( "size of frame %d\n", sizeof( sdt_frame_unit ) );

	MSDT* msdt = new MSDT();
	msdt->Initialize( N_PERLINE, 0.0001f );

	FILE *pf = fopen( "in.txt" ,"r" );
	float a[N_PERLINE + 1];

	for ( int i = 0; i < N_NUMBER; i ++ ) {
		for ( int j = 0; j < N_PERLINE; j ++ ) {
			fscanf( pf, "%f", &a[j] );
		}
		msdt->PushBack( a );
	}
	//fclose( pf );
	//fseek( pf, 0, 0 );

	msdt->Flush();
	printf( "%d\n", msdt->GetValidDataLength() );

	int length = msdt->GetValidDataLength();

	FILE *pout = fopen( "out.txt", "w" );
	for ( int i = 0; i < length; i ++ ) {
		//printf( "i = %d\n", i );
		msdt->GetData( a, i );

		/*
		for ( int j = 0; j < 10; j ++ ) {
			fscanf( pf, "%f", &a[j] );
		}
		*/
		for ( int j = 0; j < N_PERLINE; j ++ ) {
			fprintf( pout, "%5.6f\t", a[j]);
		}
	
		fprintf( pout, "\n" );
	}

	delete msdt;

	int stop_flag;
	scanf( "%d", &stop_flag );
	return 0;
}
