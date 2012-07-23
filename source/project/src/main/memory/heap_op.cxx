#include <heap_op.h>
#include <heap_interface.h>
#include <heap_system.h>
#include <heap_buddy.h>
#include <log.h>

#include <stdlib.h>

using namespace::Core;
using namespace::Core::Memory;

static heap_operations	hOps;

static heap_handle		hHeap = NULL;

void HeapOP::Init(EHeapAllocType allocType, int size) {
	Util::DebugPrintf("HeapOP Init\n");

	if (allocType == HeapOP::E_HEAP_SYSTEM) {
		fill_heap_operations_sys(&hOps);

		// system memory, no need to give the heap params 
		hHeap			= hOps.init(NULL, 0);
	}
	else if (allocType == HeapOP::E_HEAP_BUDDY) {
		fill_heap_operations_buddy(&hOps);

		// 30M total space
		void* buff		= malloc(size * sizeof(char));
		hHeap			= hOps.init(buff, size);
	}

	return;
}

void HeapOP::Deinit() {
	Util::DebugPrintf("HeapOP Deinit\n");

	hOps.destroy(hHeap);
	return;
}

void* HeapOP::operator new(std::size_t size) throw(std::bad_alloc) {
	void* buff = hOps.alloc(hHeap, (std::size_t)size);
	if (buff == NULL) throw new std::bad_alloc();

	Util::DebugPrintf("HeapOP alloc size %d on 0x%08X\n", size, *((unsigned int*)&buff));

	return buff;
}

void  HeapOP::operator delete(void* buff) throw() {
	Util::DebugPrintf("HeapOP dealloc on 0x%08X\n", *((int*)&buff));
	hOps.dealloc(hHeap, buff);
	return;
}

void* HeapOP::operator new[](std::size_t size) throw(std::bad_alloc) {
	void* buff = hOps.alloc(hHeap, (std::size_t)size);
	if (buff == NULL) throw new std::bad_alloc();
	Util::DebugPrintf("HeapOP alloc size %d on 0x%08X\n", size, *((int*)&buff));
	return buff;
}

void  HeapOP::operator delete[](void* buff) throw() {
	Util::DebugPrintf("HeapOP dealloc on 0x%08X\n", *((int*)&buff));
	hOps.dealloc(hHeap, buff);
	return;
}

#ifdef HEAPOP_DEBUG_MODE
void* HeapOP::operator new(std::size_t size, const char* file, size_t line) throw(std::bad_alloc) {
		void* buff = hOps.alloc(hHeap, (size_t)size, file, line);
		if (buff == NULL) throw new std::bad_alloc();

		Util::DebugPrintf("HeapOP alloc size %d on 0x%08X\n", size, *((unsigned int*)&buff));

		return buff;
}

void  HeapOP::operator delete(void* buff, const char* file, size_t line) throw() {
	Util::DebugPrintf("HeapOP dealloc on 0x%08X\n", *((int*)&buff));
	hOps.dealloc(hHeap, buff, file, line);
	return;
}

void* HeapOP::operator new[](std::size_t size, const char* file, size_t line) throw(std::bad_alloc) {
	void* buff = hOps.alloc(hHeap, (std::size_t)size, file, line);
	if (buff == NULL) throw new std::bad_alloc();
	Util::DebugPrintf("HeapOP alloc size %d on 0x%08X\n", size, *((int*)&buff));
	return buff;
}

void  HeapOP::operator delete[](void* buff, const char* file, size_t line) throw() {
	Util::DebugPrintf("HeapOP dealloc on 0x%08X\n", *((int*)&buff));
	hOps.dealloc(hHeap, buff, file, line);
	return;
}

#endif

/*
void* HeapOP::operator new(std::size_t size, void* p) throw(std::bad_alloc) {
}

void  HeapOP::operator delete(void* p, void* buff) throw() {
}

void* HeapOP::operator new[](std::size_t size, void* p) throw(std::bad_alloc) {
}

void  HeapOP::operator delete[](void* p, void* buff) throw() {
}
*/


