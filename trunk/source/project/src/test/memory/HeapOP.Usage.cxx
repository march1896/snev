#include <heap_op.h>
#include <log.h>

using namespace::Core::Memory;
using namespace::Core::Util;

class HeapAllocClass: public HeapOP {
};

int main() {
	//Core::Util::DebugPrintf("Hello, world!\n");
	HeapOP::Init(HeapOP::E_HEAP_BUDDY, 0);
	HeapAllocClass* tester = new HeapAllocClass();
	delete tester;
	return 0;
}
