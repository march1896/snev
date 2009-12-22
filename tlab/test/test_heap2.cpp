#include "heap2.h"
#include <vector>
#include <cstdlib>
#include <iostream>
using namespace std;

class A {
public:
	A( int i ): dummy( i ) 
	{}
	int dummy;
};

#define HEAPSIZE 1000
int main() {
	void *memory = (void*)malloc( HEAPSIZE );
	InitHeap( memory, HEAPSIZE );
	std::vector< A > v;
	v.clear();

	for ( int i = 0; i < 10; i ++ ) {
		v.push_back( A( i ) );
	}

	for ( std::vector< A >::iterator itr = v.begin(); itr != v.end(); itr ++ ) {
		cout << itr->dummy << endl;
	}

	//v.clear();

	CheckLeakPoint();
	free( memory );
	cin.get();
	return 0;
}
