//Michael Levashov
//Robotics@Maryland
//
//Used to store a single ping direction vector along with its point number

#include "math/include/Vector3.h"
#ifndef _SONAR_PING
#define _SONAR_PING

namespace ram {
namespace sonar {

struct sonarPing {
    math::Vector3 direction;
    int distance;
    int point_num; //number of points that hydrophone 0 was found at
};

} //namespace ram
} //namespace sonar
#endif
