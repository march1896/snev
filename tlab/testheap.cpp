#include "heap.h"
#include <cstdio>
#include <cstdlib>

int main() {
	int stopflag;
	CHeap1* heap = new CHeap1();
	void *memory = (void*)malloc( 2 << 16 );
	heap->CreateFromBuffer( memory, 2 << 16 );
	
	delete heap;

	scanf("%d", &stopflag );
	return 0;
}
