#ifndef _TJ_REGEXP_
#define _TJ_REGEXP_
namespace regexp {
struct Context;
Context* RegexpCompile( const char* Pattern );
bool 	RegexpAcceptable( const char* Text, const Context* con );
void 	OutputContext( Context* con );
void 	OutputDfa( Context* con );
};
#endif
