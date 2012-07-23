#include <heap_system.h>

#include <cstdlib>

#ifndef MEMORY_DEBUG
heap_handle cb_heap_init_sys(void *buff, int size) {
	return (heap_handle) NULL;
}

void cb_heap_destroy_sys(heap_handle pheap) {
	return;
}

void* cb_heap_alloc_sys(heap_handle pheap, int size) {
	return malloc((size_t)size);
}

void cb_heap_dealloc_sys(heap_handle pheap, void *buff) {
	free(buff);
}

void* cb_heap_realloc_sys(heap_handle pheap, void *buff, int size) {
	return realloc(buff, size);
}

void cb_heap_dump_sys(heap_handle pheap) {
	return;
}

#else // MEMORY_DEBUG

#include <set>
#include <multiset>
class HeapSysDebug {
public:
	void AddRecord(const void* addr, int size, const char* file, int line) {
		mAllocates.insert(Record(addr, size, file, line));

		mTotalAllocatedSize += size;
	}

	void RemoveRecord(const void* addr, const char* file, int line) {
		Record rec(addr, 0, file, line);
		std::set::const_iterator itr = mAllocates.find(rec);

		if (itr != mAllocates.end()) {
			mTotalAllocatedSize -= itr->size;
			mAllocates.erase(rec);
		}
		else {
			mDeallocates.insert(rec);
		}
	}

	int GetTotalBlockNumber() const {
		return mAllocates.size();
	}

	int GetTotalAllocatedSize() const {
		return mTotalAllocatedSize;
	}

	void DumpDetailedInforamtion() const {
		Core::Util::DebugPrintf("System Heap Dump Start.\n");
		heap_dump_helper(
				GetTotalBlockNumber(), // total allocated memory block number
				GetTotalAllocatedSize(), // total allocated memory size
				-1, // total free memory block number
				-1, // total free memory size
				-1, // largest free memory block size
				);

		Core::Util::DebugPrintf("Detailed allocates information: \n");
		for (std::set<Record>::const_iterator itr = mAllocates.begin();
				itr != mAllocates.end(); itr ++) {
			Core::Util::DebugPrintf("0x%8X (file %s, line %d)\n", *((unsigned int*)&itr->addr), itr->file, itr->line);
		}

		Core::Util::DebugPrintf("\nDetailed mis-deallocates information:\n");
		for (std::multiset<Records>::const_iterator itr = mDeallocates.begin();
				itr != mDeallocates.end(); itr ++) {
			Core::Util::DebugPrintf("0x%8X (file %s, line %d)\n", *((unsigned int*)&itr->addr), itr->file, itr->line);
		}

		Core::Util::DebugPrintf("System Heap Dump End.\n");

		return;
	}

private:

	struct Record {
		const void* addr; // address that allocated
		const int size; // size of the memory block
		const char* file;
		const int line;

		Record(const void* _addr, const int _size, const char* _file, const int _line) :
			addr(_addr), size(_size), file(_file), line(_line)
		{
		}

		bool operator < (const Record& rec) const {
			return (char*)addr < (char*)rec->addr;
		}
	};

	/* mAllocates keep records of allocated memory blocks,
	 * the record will be removed when the address is deallocated */
	std::set<Record> mAllocates;

	/* mDeallocates keep records of missing deallocating an address that 
	 * has never been allocated */
	std::multiset<Record> mDeallocates;

	/* mTatalAllocatedSize represent total allocated memory size */
	int mTotalAllocatedSize;
};

HeapSysDebug& HeapSysDebug::GetInstance() {
	static HeapSysDebug theInstance;

	return &theInstance;
}

heap_handle cb_heap_init_sys_debug(void *buff, int size) {
	// TODO: it's better not to use the system heap
	HeapSysDebug* pHeap = new pHeap();

	return (heap_handle)pHeap;
}

void cb_heap_destroy_sys_debug(heap_handle pheap) {
	delete pHeap;
	return;
}

void* cb_heap_alloc_sys_debug(heap_handle pheap, int size, const char* file, size_t line) {
	void *addr = (void*)malloc(size);
	if (addr == NULL) return NULL;

	HeapSysDebug* pHeap = (HeapSysDebug*) pheap;
	pHeap->AddRecord(addr, size, file, line);
	return addr;
}

void cb_heap_dealloc_sys_debug(heap_handle pheap, void *buff, const char* file, size_t line) {
	if (buff == NULL) return;

	free(buff);

	HeapSysDebug* pHeap = (HeapSysDebug*) pheap;
	pHeap->RemoveRecord(addr, file, line);
}

void* cb_heap_realloc_sys_debug(heap_handle pheap, void *buff, int size, const char* file, size_t line) {
	void* addr = realloc(buff, size);

	HeapSysDebug* pHeap = (HeapSysDebug*) pheap;
	pHeap->RemoveRecord(buff, NULL, 0);
	pHeap->AddRecord(addr, size, file, line);

	return addr;
}

void cb_heap_dump_sys_debug(heap_handle pheap) {
	HeapSysDebug* pHeap = (HeapSysDebug*) pheap;
	pHeap->DumpDetailedInforamtion();

	return;
}
#endif // MEMORY_DEBUG

void fill_heap_operations_sys (heap_operations* ops) {
#ifndef MEMORY_DEBUG
	ops->init = cb_heap_init_sys;
	ops->destroy = cb_heap_destroy_sys;
	ops->alloc = cb_heap_alloc_sys;
	ops->dealloc = cb_heap_dealloc_sys;
	ops->realloc = cb_heap_realloc_sys;
	ops->dump = cb_heap_dump_sys;
#else // MEMORY_DEBUG
	ops->init = cb_heap_init_sys_debug;
	ops->destroy = cb_heap_destroy_sys_debug;
	ops->alloc = cb_heap_alloc_sys_debug;
	ops->dealloc = cb_heap_dealloc_sys_debug;
	ops->realloc = cb_heap_realloc_sys_debug;
	ops->dump = cb_heap_dump_sys_debug;
#endif // MEMORY_DEBUG
}

