#ifndef _COMINC_H_
#define _COMINC_H_

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define dbg_assert assert

#if defined(__GCC__)

#include <stdbool.h>

#elif defined(_MSC_VER)

typedef int bool;
#define false 0
#define true 1

#define inline __inline

#else
/* MinGW make does not contains __GCC__ macro */

#include <stdbool.h>

#endif


#define halloc malloc
#define hfree free

#endif /* _COMINC_H_ */
