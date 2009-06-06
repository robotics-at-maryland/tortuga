//A number of utility functions used by the sonar code
//Should be included after Sonar.h

#ifndef _RAM_SONAR_UTILS
#define _RAM_SONAR_UTILS

namespace ram {
namespace sonar {

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

} // namespace sonar
} // namespace ram

#endif
