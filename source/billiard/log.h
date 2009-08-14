#ifndef _LOG_H_
#define _LOG_H_
#include <cstdio>
class Log
{
private:
	Log();
public:
	static void print( char *message );
};

#endif
