//A number of utility functions used by the sonar code
//Should be included after Sonar.h

#ifndef _RAM_SONAR_UTILS
#define _RAM_SONAR_UTILS

namespace ram {
namespace sonar {

adcmath_t myAbs(adcmath_t x);
int gcd(int a, int b);
template<class T> int8_t compare(T a, T b);
template<class T> int8_t sign(T a);

} // namespace sonar
} // namespace ram

#endif
