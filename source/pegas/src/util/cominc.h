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

/* inline related */
#ifdef __NO_INLINE
#define inline
#else
#define inline __inline
#endif

#ifdef _DEBUG
#define dbg_assert(x) assert(x)
#else 
#define dbg_assert(x)
#endif

#else
/* MinGW make does not contains __GCC__ macro */

#include <stdbool.h>
#define dbg_assert assert

#endif

/* handles the 32, 64bit pointers related problem */
#include <stdint.h>

#define unused(x) (void)(x)

#define container_of(ptr, type, member) \
	(type *)((char *)ptr - offsetof(type,member))

extern inline void rt_error(const char* last_words);

#endif /* _COMINC_H_ */
