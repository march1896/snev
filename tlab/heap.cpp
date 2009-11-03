#include "heap.h"
#include <cstdlib>

#define ALIGNDOWN( x, ALIGN_T ) ( (x) & ~( ALIGN_T - 1 ))
#define ALIGNUP( x, ALIGN_T ) ( ( (x) + ALIGN_T - 1 ) & ~( ALIGN_T - 1 ) )

#define NODESIZE sizeof( CHeap1::Node )
#define SPLIT_THRESHOLD 32

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
	Node *node = pSentinel->pNextFree;

	while ( node != NULL ) {
		if ( node->GetDataSize() > size ) {
			// found a suitable node
			break;
		}
		node = node->pNextFree;
	}

	if ( node == NULL ) {
		return NULL;
	}

	Node *NextFree = node->pNextFree;
	Node *PrevFree = node->pPrevFree;
	Node *NextMem = node->pNextMem;
	Node *PrevMem = node->pPrevMem;

	if ( node->GetDataSize() - size < SPLIT_THRESHOLD ) {
		// will not divide
		PrevFree->pNextFree = NextFree;
		if ( NextFree ) NextFree->pPrevFree = PrevFree;	

		node->pPrevFree = node->pNextFree = NULL;
	}
	else {
		void* addr = (void*)( (char*) node + size + sizeof( Node ) );
		addr = (void*)ALIGNUP( (uint)addr, AlignMent );

		Node* new_node = (Node*)( addr );
		new_node->pPrevMem = node;
		new_node->pNextMem = NextMem;
		new_node->pPrevFree = PrevFree;
		new_node->pNextFree = NextFree;

		if ( PrevFree ) PrevFree->pNextFree = new_node;
		if ( NextFree ) NextFree->pPrevFree = new_node;

		if ( NextMem && NextMem != pHeapEnd ) NextMem->pPrevMem = new_node;
		node->pNextMem = new_node;

		node->pPrevFree = node->pNextFree = NULL;
	}
	
	return (void*)( (char*)node + sizeof( Node ) );
}

void CHeap1::Free( void* mem ) {
	Node* Tofree = (Node*)( (char*)mem - sizeof(Node) );
	mem = NULL;

	if ( !CheckNode( Tofree ) ) {
		// This is not a vilide node, something wrong
		return;
	}

	// because sentinel is always exsit as a free node, so any node in freelist will at least has one p*Free != NULL
#define ISFREE( node ) ( CheckNode( node ) && node->pPrevFree != NULL || node->pNextFree != NULL )
	Node* Prev = Tofree->pPrevMem;
	Node* Next = Tofree->pNextMem;
	Node* node = Tofree;

	if ( Prev == pSentinel ) {
		if ( ISFREE( Next ) ) {
			node->pPrevFree = Next->pPrevFree;
			node->pNextFree = Next->pNextFree;

			Next->pPrevFree->pNextFree = node;
			if ( Next->pNextFree ) Next->pNextFree->pPrevFree = node;

			node->pNextMem = Next->pNextMem;
			if ( Next->pNextMem <= pHeapEnd ) node->pNextMem->pPrevMem = node;
		}
		else {
			Tofree->pNextFree = pSentinel->pNextFree;
			Tofree->pPrevFree = pSentinel;
			if ( pSentinel->pNextFree ) pSentinel->pNextFree->pPrevFree = Tofree;
			pSentinel->pNextFree = Tofree;
		}
		return;
	}

	if ( ISFREE( Prev ) && ISFREE( Next ) ) {
		// Prev and Next both in FreeList
		Next->pPrevFree->pNextFree = Next->pNextFree;
		if ( Next->pNextFree ) Next->pNextFree->pPrevFree = Next->pPrevFree;

		Prev->pNextMem = Next->pNextMem;
		if ( Next->pNextMem <= pHeapEnd ) Next->pNextMem->pPrevMem = Prev;
	}
	else if ( ISFREE( Prev ) ) {
		Prev->pNextMem = node->pNextMem;
		if ( node->pNextMem <= pHeapEnd ) node->pNextMem->pPrevMem = Prev;
	}
	else if ( ISFREE( Next ) ) {
		node->pPrevFree = Next->pPrevFree;
		node->pNextFree = Next->pNextFree;

		Next->pPrevFree->pNextFree = node;
		if ( Next->pNextFree ) Next->pNextFree->pPrevFree = node;

		node->pNextMem = Next->pNextMem;
		if ( Next->pNextMem <= pHeapEnd ) node->pNextMem->pPrevMem = node;
	}
	else {
		// merge failed
		Tofree->pNextFree = pSentinel->pNextFree;
		Tofree->pPrevFree = pSentinel;
		if ( pSentinel->pNextFree ) pSentinel->pNextFree->pPrevFree = Tofree;
		pSentinel->pNextFree = Tofree;
	}
	return;
#undef ISFREE
}

bool CHeap1::CheckNode( Node* tocheck ) {
	if ( tocheck == NULL ) return false;
	if ( tocheck < ( Node*)pSentinel || tocheck >= (Node*)pHeapEnd ) return false;
#define CHECKPOINTER( pointer ) pointer == NULL || ( pointer >= (Node*)pSentinel && pointer <= (Node*)pHeapEnd )
	bool ret = true;
	ret &= CHECKPOINTER( tocheck->pPrevMem );
	ret &= CHECKPOINTER( tocheck->pNextMem );
	ret &= CHECKPOINTER( tocheck->pPrevFree );
	ret &= CHECKPOINTER( tocheck->pNextFree );
#undef CHECKPOINTER
	return ret;
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

uint CHeap1::GetBlockSize( void* Mem ) const { 
	if ( Mem == NULL ) return 0;
	Node* node = (Node*)( (char*) Mem - sizeof( Node ) );
	return node->GetDataSize();
}

#ifdef MEM_DEBUG
#include <cstdio>
void CHeap1::OutputFreeList() const {
	Node* node = pSentinel->pNextFree;
	while ( node ) {
		printf( "ADDR 0x%08x, SIZE 0x%08x\n", (uint)node, node->GetSize() );
		node = node->pNextFree;
	}
	return;
}
#endif //MEM_DEBUG

