#ifndef _TJ_REGEXP_
#define _TJ_REGEXP_
namespace regexp {
struct Context;
Context* RegexpCompile( const char* Pattern );
bool 	RegexpAcceptable( const char* Text, const Context* dfa );
void 	OutputContext( Context* con );
};
#endif
