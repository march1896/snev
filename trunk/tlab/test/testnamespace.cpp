#include <cstdio>

namespace tangel {
int add( int *a, int *b );
};

using namespace tangel;
int tangel::add( int *a, int *b ) {
	return *a + *b;
}

int main() {
	int a = 5, b = 7;
	std::printf( "%d\n", tangel::add( &a, &b ) );
	std::scanf( "%d", &a );
	return 0;
}
