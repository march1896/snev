#include <cmath>
#include "zmath.h"

const float EPS = 0.000001;
bool float_equal( float lhs, float rhs ) {
	if ( fabs( lhs - rhs ) < EPS ) return true;
	else return false;
}
