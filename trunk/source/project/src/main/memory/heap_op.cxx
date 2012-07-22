#include <heap_op.h>
#include <heap_interface.h>
#include <heap_system.h>
#include <heap_buddy.h>
#include <log.h>

#include <stdlib.h>

using namespace::Core;
using namespace::Core::Memory;

static heap_operations	hOps = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static heap_handle		hHeap = NULL;

void HeapOP::Init(EHeapAllocType allocType, int size) {
	Util::DebugPrintf("HeapOP Init\n");
	if (allocType == HeapOP::E_HEAP_SYSTEM) {
		hOps.init		= cb_heap_init_sys;
		hOps.destroy	= cb_heap_destroy_sys;
		hOps.alloc		= cb_heap_alloc_sys;
		hOps.dealloc	= cb_heap_dealloc_sys;
		hOps.realloc	= cb_heap_realloc_sys;

		// system memory, no need to give the heap params 
		hHeap			= hOps.init(NULL, 0);
	}
	else if (allocType == HeapOP::E_HEAP_BUDDY) {
		/*
		hOps.init		= cb_heap_init_buddy;
		hOps.destroy	= cb_heap_destroy_buddy;
		hOps.alloc		= cb_heap_alloc_buddy;
		hOps.dealloc	= cb_heap_dealloc_buddy;
		hOps.realloc	= cb_heap_realloc_buddy;

		// 30M total space
		void* buff		= malloc(size * sizeof(char));
		hHeap			= hOps.init(buff, size);
		*/
	}
	else {
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
	Util::DebugPrintf("HeapOP alloc size %d on %0x8d\n", size, *((int*)buff));
	return buff;
}

void  HeapOP::operator delete(void* buff) throw() {
	Util::DebugPrintf("HeapOP dealloc on %0x8d\n", *((int*)buff));
	hOps.dealloc(hHeap, buff);
	return;
}

void* HeapOP::operator new[](std::size_t size) throw(std::bad_alloc) {
	void* buff = hOps.alloc(hHeap, (std::size_t)size);
	if (buff == NULL) throw new std::bad_alloc();
	Util::DebugPrintf("HeapOP alloc size %d on %0x8d\n", size, *((int*)buff));
	return buff;
}

void  HeapOP::operator delete[](void* buff) throw() {
	Util::DebugPrintf("HeapOP dealloc on %0x8d\n", *((int*)buff));
	hOps.dealloc(hHeap, buff);
	return;
}

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


