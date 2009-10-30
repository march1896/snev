#ifndef _TJ_HEAP_
#define _TJ_HEAP_

// TODO: remove this
#define uint unsigned int

class IHeap {
public:
	virtual ~IHeap();
	virtual bool CreateFromBuffer( void* buffer, uint size ) = 0;
	virtual void Destroy() = 0;
	virtual void* Alloc( uint size ) = 0;
	virtual void Free( void *mem ) = 0;
	virtual uint GetLargestFree() = 0;
	virtual uint GetTotalFree() = 0;
};

class CHeap1: public IHeap {
public:
	CHeap1( const uint AlignMent = 32 );
	~CHeap1();

	bool CreateFromBuffer( void* buffer, uint size );
	void Destroy();
	void* Alloc( uint size );
	void Free( void* mem );
	uint GetLargestFree() const;
	uint GetTotalFree() const;
	uint GetHeapSize() const { return (uint)( (char*)pHeapEnd - (char*)pSentinel ) };
protected:
	class Node {
	public:
		Node* pPrevMem;
		Node* pNextMem;
		Node* pPrevFree;
		Node* pNextFree;

		uint GetSize() const {
			return (uint)pNextMem - (uint)this;
		}
		uint GetDataSize() const {
			return GetSize() - sizeof( Node );
		}
	#ifdef MEM_DEBUG
		uint CallerLine;
		const char* CallerFile;
	#endif
	};

	const uint AlignMent;
	bool Initialized;
	Node* pSentinnel; // void* pHeapStart; // start and end memory after alignment
	void* pHeapEnd;  // uint HeapSize; 

	void* pHeapMemoryStart;// because the start node is aligned, if we want to remove this heap, we need to remember this;
	uint TotalFree;
};
#endif // _TJ_HEAP_
