#include "sdt_def.h"
#include <cstdlib>

// the algorithm is copied from
// http://www.fox-toolkit.org/ftp/fasthalffloatconversion.pdf
static u32 mantissatable[ 2048 ];
static u32 exponenttable[ 64 ];
static u32 offsettable[ 64 ];
static u32 basetable[ 1024 ];
static u32 shifttable[ 512 ];

static u32 convertmantissa(u32 i);
static void generatetables();

void hlaf_float_init() {
	mantissatable[ 0 ]  = 0;
	for ( int i = 1; i < 1024; i ++ ) {
		mantissatable[i] = convertmantissa( i );
	}
	for ( int i = 1024; i < 2048; i ++ ) {
		mantissatable[i] = 0x38000000 + ((i-1024)<<13);
	}

	exponenttable[0] = 0;
	for ( int i = 1; i < 30; i ++ ) {
		exponenttable[i] = i<<23;
	}
	exponenttable[31]= 0x47800000;
	exponenttable[32]= 0x80000000;
	for ( int i = 33; i < 63; i ++ ) {
		exponenttable[i] = 0x80000000 + (i-32)<<23;
	}
	exponenttable[63]= 0xC7800000;

	for ( int i = 0; i < 64; i ++ ) {
		offsettable[i] = 1024;
	}

	offsettable[0] = 0;
	offsettable[32]= 0;

	generatetables();
}

void generatetables(){
	u32 i;
	int e;
	for(i=0; i<256; ++i){
		e=i-127;
		if(e<-24){ // Very small numbers map to zero
			basetable[i|0x000]=0x0000;
			basetable[i|0x100]=0x8000;
			shifttable[i|0x000]=24;
			shifttable[i|0x100]=24;
		}
		else if(e<-14){ // Small numbers map to denorms
			basetable[i|0x000]=(0x0400>>(18-e));
			basetable[i|0x100]=(0x0400>>(18-e)) | 0x8000;
			shifttable[i|0x000]=-e-1;
			shifttable[i|0x100]=-e-1;
		}
		else if(e<=15){ // Normal numbers just lose precision
			basetable[i|0x000]=((e+15)<<10);
			basetable[i|0x100]=((e+15)<<10) | 0x8000;
			shifttable[i|0x000]=13;
			shifttable[i|0x100]=13;
		}
		else if(e<128){ // Large numbers map to Infinity
			basetable[i|0x000]=0x7C00;
			basetable[i|0x100]=0xFC00;
			shifttable[i|0x000]=24;
			shifttable[i|0x100]=24;
		}
		else{ // Infinity and NaN's stay Infinity and NaN's
			basetable[i|0x000]=0x7C00;
			basetable[i|0x100]=0xFC00;
			shifttable[i|0x000]=13;
			shifttable[i|0x100]=13;
		}
	}
}

u32 convertmantissa(u32 i){
	u32 m=i<<13; // Zero pad mantissa bits
	u32 e=0; // Zero exponent
	while(!(m&0x00800000)){ // While not normalized
		e-=0x00800000; // Decrement exponent (1<<23)
		m<<=1; // Shift mantissa
	}
	m&=~0x00800000; // Clear leading 1 bit
	e+=0x38800000; // Adjust bias ((127-14)<<23)
	return m | e; // Return combined number
}

f32 slope2float( sdt_slope h ) {
	//return f32( in ) / 1000.0f;
	u32 ff = mantissatable[offsettable[h>>10]+(h&0x3ff)]+exponenttable[h>>10] | ((h&0x8000)<<16);
	//u32 ff = ((h&0x8000)<<16) | (((h&0x7c00)+0x1C000)<<13) | ((h&0x03FF)<<13);
	f32 f = *((f32*)(&ff));
	return f;
}

sdt_slope float2slope( f32 ff ) {
	//assert( in < 32.0f && in > -32.0f );
	u32 f = *((u32*)(&ff));

	sdt_slope s = basetable[(f>>23)&0x1ff]+((f&0x007fffff)>>shifttable[(f>>23)&0x1ff]) | ((f>>16)&0x8000);
	return s;
}

sdt_data sdt_frame_get( sdt_frame* pf, sdt_length l ) {
	f32 x = slope2float( pf->slope );
	return pf->start + l * x;
}

void sdt_frame_unit_init( sdt_frame_unit* pfu ) {
	if ( pfu == NULL ) return;
	pfu->next = NULL;
	pfu->start = 0;
	pfu->end = 0;
	pfu->n_data = 0;
}

// below is the implementation of sdt_f16

// below is the implementation of frame
/*
   sdt_header frame_get_header( sdt_frame* f ) {
   return *( (sdt_header*)f );
   }

   sdt_length frame_get_length( sdt_frame* f ) {
   return *( ((sdt_header*)f) + 1 );
   }

   sdt_f16 frame_get_slope( sdt_frame* f ) {
   return *( ((sdt_f16*)f) + 1 );
   }	

   sdt_f32 frame_get_start( sdt_frame* f ) {
   return *( ((sdt_f32*)f) + 1 );
   }

   void frame_set_header( sdt_frame* f, sdt_header h ) {
   sdt_header* addr = (sdt_header*)f;
 *addr = h;
 }

 void frame_set_length( sdt_frame* f, sdt_length l ) {
 sdt_length* addr = ((sdt_header*)f) + 1;
 *addr = l;
 }

 void frame_set_slope( sdt_frame* f, sdt_f16 slope ) {
 sdt_f16* addr = ((sdt_f16*)f) + 1;
 *addr = slope;
 }	

 void frame_set_start( sdt_frame* f, sdt_f32 start ) {
 sdt_f32* addr = ((sdt_f32*)f) + 1;
 *addr = start;
 }
 */

