#ifndef _REG_READER_
#define _REG_READER_

typedef struct __s_regc {
	        int* buffer;
		        int size;
} s_regc, *p_regc;

p_regc regc_new();

void regc_del( p_regc prc );

void regc_print( p_regc prc );

p_regc regc_compile_from_memory( const char* str );

p_dfa build_dfa_from_memory( char* str );

#endif // _REG_READER_
