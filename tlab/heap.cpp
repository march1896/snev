#include "heap.h"
#include <cstdlib>

#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( ALIGN_T - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + ALIGN_T - 1 ) & ~( ALIGN_T - 1 ) )

#define NODESIZE sizeof( CHeap1::Node )
#define SPLIT_THRESHOLD 64

CHeap1::CHeap1( const uint alignment ):
	AlignMent( alignment ),
	Initialized( false ),
	pSentinel( NULL ),
	pHeapMemoryStart( NULL ),
	TotalFree( 0 )
{
}

CHeap1::~CHeap1() {
	Destroy();
}

bool CHeap1::CreateFromBuffer( void* buffer, uint size ) {
	pHeapMemoryStart = buffer;
	void *_start = (void*) ALIGNUP( (uint)buffer, AlignMent );
	void *_end = (void*) ALIGNDOWN( (uint)buffer + size, AlignMent );
	pHeapEnd = _end;

	pSentinel = (Node*)_start;
	Node* new_node = (Node*)_start + 1;

	pSentinel->pPrevMem = NULL;
	pSentinel->pNextMem = new_node;
	pSentinel->pPrevFree = NULL;
	pSentinel->pNextFree = new_node;

	new_node->pPrevMem = pSentinel;
	new_node->pNextMem = (Node*)_end;
	new_node->pPrevFree = pSentinel;
	new_node->pNextFree = NULL;
}

void CHeap1::Destroy() {
	if ( !Initialized ) return;
	// remove this buffer use system default memory manager
	free( pHeapMemoryStart );

	pSentinel = NULL;
	Initialized = false;
}

void* CHeap1::Alloc( uint size ) {
	Node *Next = pSentinel->pNextFree;
	Node *Prev = NULL;

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

	if ( Next->GetDataSize() - size < SPLIT_THRESHOLD ) {
		// will not divide
		Prev->pNextFree = Next->pNextFree;
		if ( Next->pNextFree ) Next->pNextFree->pPrevFree = Prev;	

		Next->pPrevFree = Next->pNextFree = NULL;
	}
	else {
		void* addr = (void*)( (char*) Next + size + sizeof( Node ) );
		addr = (void*)ALIGNUP( (uint)addr, AlignMent );
		Node* new_node = (Node*)( addr );
		new_node->pPrevMem = Next;
		new_node->pNextMem = Next->pNextMem;
		new_node->pPrevFree = Prev;
		new_node->pNextFree = Next->pNextFree;

		Next->pNextMem = new_node;

		Prev->pNextFree = new_node;
		if ( Next->pNextFree ) Next->pNextFree->pPrevFree = new_node;
	}
	
	return (void*)( (char*)Next + sizeof( Node ) );
}

void CHeap1::Free( void* mem ) {
	Node* Tofree = (Node*)( (char*)mem - sizeof(Node) );

	if ( !CheckNode( Tofree ) ) {
		// This is not a vilide node, something wrong
		return;
	}

	// because sentinel is always exsit as a free node, so any node in freelist will at least has one p*Free != NULL
#define ISFREE( node ) node->pPrevFree != NULL || node->pNextFree != NULL
	Node* Next = Tofree->pNextMem;
	if ( ISFREE( Tofree->pPrevMem ) ) {
		Tofree = Merge( Tofree->pPrevMem, Tofree );
	}
	if ( ISFREE( Next ) ) {
		Tofree = Merge( Tofree, Next );
	}
#undef ISFREE
	return;
}

CHeap1::Node* CHeap1::Merge( Node* Prev, Node* Next ) {
	Prev->pNextFree = Next->pNextFree;
	Next->pNextFree->pPrevFree = Prev;
	Prev->pNextMem = Next->pNextMem;
}

bool CHeap1::CheckNode( Node* tocheck ) {
#define CHECKPOINTER( pointer ) pointer == NULL || ( pointer >= pSentinel && pointer <= (Node*)pHeapEnd )
	CHECKPOINTER( tocheck->pPrevMem );
	CHECKPOINTER( tocheck->pNextMem );
	CHECKPOINTER( tocheck->pPrevFree );
	CHECKPOINTER( tocheck->pNextFree );
#undef CHECKPOINTER
}

uint CHeap1::GetLargestFree() const  {
	Node* ret = pSentinel->pNextFree;
	uint size = 0;
	while ( ret != NULL ) {
		if ( ret->GetDataSize() > size ) size = ret->GetDataSize();
	}
	return size;
}

uint CHeap1::GetTotalFree() const {
	return TotalFree;
}

