//A number of utility functions used by the sonar code
#include "Sonar.h"

using namespace ram::sonar;

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

template<class T>
int8_t compare(T a, T b)
{
    if (a > b) return 1;
    else if (b < a) return -1;
    else return 0;
}


template<class T>
int8_t sign(T a)
{
    return compare(a, 0);
}
