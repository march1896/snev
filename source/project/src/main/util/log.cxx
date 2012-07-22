#include <log.h>

#include <cstdio>
#include <cstdarg>

/*
Shit this, if use pure functions with namespace. 
we can not directly using namespace like this,
using namespace::Core::Util;
int DebugPrintf(const char* format, ... ) {

The linker will not find the implementation of this function.
instead, we should write like below.
*/
namespace Core {
	namespace Util {
		int DebugPrintf(const char * format, ... ) {
			va_list arglist;

			printf("Debug: ");
			va_start(arglist, format);
			vprintf(format, arglist);
			va_end(arglist);

			return 0;
		}
	}
}

