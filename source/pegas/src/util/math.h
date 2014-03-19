#ifndef _WHAT_A_MATH_H_
#define _WHAT_A_MATH_H_

/* micro soft seems have defined these macros in stdlib.h */
#ifndef min
#define min(x, y) (x<y)?(x):(y)
#endif

#ifndef max
#define max(x, y) (x>y)?(x):(y)
#endif

#include <util/bit_math.h>

#endif /* _WHAT_A_MATH_H_ */
