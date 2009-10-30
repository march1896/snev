#include <cstdlib>
#include <cstdio>
#include <cstring>

struct S {
	unsigned x;
	unsigned y;
	unsigned z;
};

int main() {
	void* buffer = (void*) malloc( 1000 );
	memset( buffer, 0, 1000 );
	void* p = (char*)buffer + 0x100;
	S* m = (S*) p;

	printf( "Buffer addr: 0x%08X\n", (unsigned)buffer );
	printf( "p addr     : 0x%08X\n", (unsigned)p );

	printf( "%u %u %u\n", *((unsigned*)p), *((unsigned*)p + 1), *((unsigned*)p + 2) );
	printf( "%u %u %u\n", *((unsigned*)p), *((unsigned*)p - 1), *((unsigned*)p - 2) );

	m->x = 0x1;
	m->y = 0x10;
	m->z = 0x100;

	printf( "%u %u %u\n", *((unsigned*)p), *((unsigned*)p + 1), *((unsigned*)p + 2) );
	printf( "%u %u %u\n", *((unsigned*)p), *((unsigned*)p - 1), *((unsigned*)p - 2) );

	int input;
	scanf( "%d", input );

	free( buffer );
	return 0;
}
