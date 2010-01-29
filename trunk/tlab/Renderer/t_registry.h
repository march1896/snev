#ifndef _TRD_REGISTER_
#define _TRD_REGISTER_

#include "t_types.h"
/*
	I want to make a big state group for storage infomation, this is why I make this register.
for the renderer state the infomation in the format of 'key/value', one fixed-size key and variable 
value. The tranditional implementation is to use a STL map like data structure. But in this register
I have no requirement to reg a value at run-time, all register can be done in compile-time. So, in 
fact, we can use a global Context instead of the register mode. So I use a global context to simulate
a (compile-time reg only) register.
*/

#define REGISTRY_Reg( key, value_type ) \
	value_type key

#define REGISTRY_Set( key, value ) \
	gl_context->##key = value
	
#define REGISTRY_Get( key, value ) \
	value = gl_context->##key

typedef struct _GLOBAL_CONTEXT {
	REGISTRY_Reg( compile_mode, BOOL ); 
	REGISTRY_Reg( displaylist, DISPLAYLISTPtr ); 
	REGISTRY_Reg( ins_context, INSCONTEXTPtr ); 
} GLCONTEXT;

/*
struct _GLOBAL_CONTEXT {
	BOOL 				compile_mode;
	DISAPLAYLIST* 		displaylist;
	INSCONTEXT* 		ins_context; // instruction context 
};
*/

extern GLCONTEXT* gl_context;

void GLCONTEXT_Init( GLCONTEXT* cont );
void GLCONTEXT_Destroy( GLCONTEXT* cont );

#endif // _TRD_REGISTER_
