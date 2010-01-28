#ifndef _TRD_REGISTER_
#define _TRD_REGISTER_

/*
	I want to make a big state group for storage infomation, this is why I make this register.
for the renderer state the infomation in the format of 'key/value', one fixed-size key and variable 
value. The tranditional implementation is to use a STL map like data structure. But in this register
I have no requirement to reg a value at run-time, all register can be done in compile-time. So, in 
fact, we can use a global Context instead of the register mode. So I use a global context to simulate
a (compile-time reg only) register.
*/

#define REG_Reg( key, value_type ) \
	value_type key;

#define REG_Set( key, value ) \
	gl_context->##key = *((key*)value); 
	
#define REG_Get( key, value ) \
	*((key*)value) = gl_context->##key;


typedef _global_context {
	REG_Reg( name, char );
	REG_Reg( age, int );
} Context;

extern Context* gl_context;

#endif // _TRD_REGISTER_
