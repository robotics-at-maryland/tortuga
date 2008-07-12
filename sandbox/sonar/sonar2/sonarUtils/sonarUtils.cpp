//A number of utility functions used by the sonar code
//Should be included after Sonar.h

#ifndef _RAM_SONAR_UTILS
#define _RAM_SONAR_UTILS

#include "Sonar.h"

namespace ram {
namespace sonar {

adcmath_t myAbs(adcmath_t x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

int gcd(int a, int b)
{
    assert(a > 0 && b > 0);
    if (b == 0)
        return a;
    else
        return gcd(b, a % b);
}

} // namespace sonar
} // namespace ram

#endif
