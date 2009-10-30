#include "heap.h"

#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( ALIGN_T - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + ALIGN_T - 1 ) & ~( ALIGN_T - 1 ) )

#define NODESIZE sizeof( CHeap1::Node );
#define SPLIT_THREAHHOLD 64

CHeap1::CHeap1( const uint alignment ):
	AlignMent = alignment,
	Initialized = false,
	pSentinel = NULL,
	pHeapMemoryStart = NULL,
	TotalFree = 0
{
}

CHeap1::CHeap1() {
	Destroy();
}

bool CHeap1::CreateFromBuffer( void* buffer, uint size ) {
	pHeapMemoryStart = buffer;
	void *_start = ALIGNUP( buffer, AlignMent );
	void *_end = ALIGNDOWN( buffer + size );
	pHeapEnd = _end;

	pSentinel = (Node*)_start;
	Node* new_node = (Node*)_start + 1;

	pSentinel->pPrevMem = NULL;
	pSentinel->pNextMem = new_node;
	pSentinel->pPrevFree = NULL;
	pSentinel->pNextFree = new_node;

	new_node->pPrevMem = pSentinel;
	new_node->pNextMem = _end;
	new_node->pPrevFree = NULL;
	new_node->pNextFree = NULL;
}

void CHeap1::Destroy() {
	// remove this buffer use system default memory manager
	free( pHeapMemoryStart );

	pSentinel = NULL;
	Initialized = false;
}

void* CHeap1::Alloc( uint size ) {
	Node *Next = pSentinel->pNextFree;
	Node *Prev;

	while ( Next != NULL ) {
		if ( Next->GetDataSize() > size ) {
			// found a suitable node
			break;
		}
		Prev = Next;
		Next = Next->pNextFree;
	}

	if ( Next == NULL ) {
		return NULL;
	}
}

void CHeap1::Free( void* mem ) {

}

uint CHeap1::GetLargestFree() {

}

uint CHeap1::GetTotalFree() {
	return TotalFree;
}

