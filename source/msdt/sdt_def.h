#ifndef _SDT_DEF_
#define _SDT_DEF_

#define __PC__
#ifdef __PC__
typedef unsigned short u16;
typedef float f32;
typedef unsigned int u32;
#endif
typedef u16 sdt_length;
typedef unsigned short sdt_slope;
typedef float sdt_data;

typedef char* sdt_address;


#define MIN_SLOPE -32.767
#define MAX_SLOPE 32.767

#define MAX_LENGTH 128

/*
   because short can represent -32768 ~ 32767, and we need 
   the accuracy of 0.001, so short can only represent 
   -32.768 ~ 32.767
   */
f32 slope2float( sdt_slope in );
sdt_slope float2slope( f32 in );

/* 
   for we use sdt_f16( short ) for slope, assume the distance of 
   x-axis is 1, the y-axis distance of two adjacent points can not
   extend the scope of sdt_f16( short ), so the difference of two 
   points can not be greater then 32.768.

   this number is large enough for this application.
   if you need more general implementation, just modify the defination
   of sdt_length, sdt_slope, sdt_data, but the size of frame will 
   increace also.
   */

struct sdt_frame {
	sdt_length 				length;
	sdt_slope 	 			slope;
	sdt_data 		 		start;
};

void hlaf_float_init();
sdt_data sdt_frame_get( sdt_frame* pf, sdt_length l );

#define FRAME_PER_UNIT 8
struct sdt_frame_unit {
	sdt_frame_unit* 		next; 			
	u16 					start;   // frames[ start ] is the first used frame in this unit
	u16 					end; 	 // the same as start
	int 					n_data;  // number of data stored in this unit
	sdt_frame 				frames[ FRAME_PER_UNIT ];
};

void sdt_frame_unit_init( sdt_frame_unit* pfu );

#endif  // _SDT_DEF_
