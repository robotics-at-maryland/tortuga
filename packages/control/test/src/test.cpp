// STD Includes
#include <iostream>

// Project Files
#include "Controller.h"
#include "IMUFunctions.h"
#include "joeMath.h"


using namespace std;

int main()
{
    cout << "Hello World" << endl;
//    translationalController();


    double magReading[3] = {-0.0764, -0.0043, -0.1848};
    double accelReading[3] = {0, -0.1736, -0.9848};
    double quaternion[4];
    quaternionFromIMU(magReading,accelReading,-CONSTANTmagneticPitchAngle*PI/180,
                      &quaternion[0]);

    std::cout << "quaternion = " << quaternion[0] << " " << quaternion[1]
                << " " << quaternion[2] << " " << quaternion[3] << std::endl;

    /*
    double crossProd[3];
    crossProduct3x1by3x1(magReading,accelReading,crossProd);
    std::cout << "cross = " << crossProd[0] << " " << crossProd[1] << " " << crossProd[2] << std::endl;
    */

}
