#ifndef _COMINC_H_
#define _COMINC_H_

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


#if defined(__GCC__)

#include <stdbool.h>

#define dbg_assert assert

#elif defined(_MSC_VER)

typedef int bool;
#define false 0
#define true 1

#define inline __inline

#pragma warning(disable : 4101) 
#define dbg_assert(x) \
 	void* _UNREFERENCED_DUMMY_

/*
#define dbg_assert(condition)  \
	do { \
		assert((condition)); \
	} while (0)
	*/

#else
/* MinGW make does not contains __GCC__ macro */

#include <stdbool.h>

#define dbg_assert assert

#endif


#define halloc malloc
#define hfree free

#endif /* _COMINC_H_ */
