#ifndef _CS_PLATFORM_
#define _CS_PLATFORM_

#define __PC__
//#define __WII__

#ifdef __PC__
	#define u32 unsigned int
	#define f32 float
	#define OSReport printf
	#define NW4R_ASSERT assert
	#define NW4R_POINTER_ASSERT assert
#elif __WI__
	#define printf OSReport
	#define assert NW4R_ASSERT
#endif

#endif //_CS_PLATFORM_
