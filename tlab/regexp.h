#ifndef _TJ_REGEXP_
#define _TJ_REGEXP_
namespace regexp {
struct Dfa;
Dfa* 	BuildDFA( const char* Pattern );
bool 	Acceptable( const char* Text, const Dfa& dfa );
bool 	FirstMatch( const char* Text, const Dfa& dfa, char* start, char* end );
};
#endif
