#include "math/include/Vector3.h"
//Used to store a single ping direction vector along with its point number

#ifndef _SONAR_PING
#define _SONAR_PING

namespace ram {
namespace sonar {

struct sonarPing {
    math::Vector3 direction;
    int point_num;
};

} //namespace ram
} //namespace sonar
#endif
