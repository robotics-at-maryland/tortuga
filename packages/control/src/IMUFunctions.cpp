/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Gland
 * All rights reserved.
 *
 * Authory: Joseph Gland <jgland@umd.edu>
 * File:  packages/control/src/Controller.cpp
 */


#include "IMUFunctions.h"
#include "joeMath.h"
#include <iostream>


//write function here that goes from IMU coord frame to vehicle coord frame

/*******************************************************************************
extracts a quaternion from IMU data

this code assumes that all input data (mag and accel) are expressed in the
vehicle's coordinate frame, not the IMU's coordinate frame



q - a quaternion with the parameterization:

                   [q1] = [e1*sin(et/2)]
               q = |q2|   |e2*sin(et/2)|
                   |q3|   |e3*sin(et/2)|
                   [q4]   [  cos(et/2) ]

               where euler axis = [e1,e2,e3] and euler angle = et
*/
void quaternionFromIMU(double mag[3],
                       double accel[3],
                       double magneticPitch,
                       double * quaternion){
    double xComponent[3];//top row of DCM
    double yComponent[3];//middle row of DCM
    double zComponent[3];//bottom row of DCM
    double temp[3];
    double rotationMatrix[3][3];//used to account for magnetic inclination
    double DCM[3][3];//direction cosine matrix

    //set xComponent equal to the normalized magnetometer reading
    //load the values
    xComponent[0] = mag[0];
    xComponent[1] = mag[1];
    xComponent[2] = mag[2];
    //normalize
    normalize3x1(xComponent);

    std::cout << "xComponent = " << xComponent[0] << " " << xComponent[1] << " " << xComponent[2] << std::endl;

    //find yComponent
    //first normalize accel
    temp[0] = accel[0];
    temp[1] = accel[1];
    temp[2] = accel[2];
    normalize3x1(temp);
    //cross product m x g
    crossProduct3x1by3x1(xComponent,temp,yComponent);
    //normalize the yComponent
    normalize3x1(yComponent);

    std::cout << "yComponent = " << yComponent[0] << " " << yComponent[1] << " " << yComponent[2] << std::endl;

    //find zComponent
    crossProduct3x1by3x1(xComponent,yComponent,zComponent);
    //normalize just for shits and giggles
    normalize3x1(zComponent);

    std::cout << "zComponent = " << zComponent[0] << " " << zComponent[1] << " " << zComponent[2] << std::endl;

    //place x, y, and z components in the direction cosine matrix
  /*  double * pDCM = &DCM[0][0];

    *(pDCM) = xComponent[0];
    *(pDCM+1) = xComponent[1];
    *(pDCM+2) = xComponent[2];
    *(pDCM+3) = yComponent[0];
    *(pDCM+4) = yComponent[1];
    *(pDCM+5) = yComponent[2];
    *(pDCM+6) = zComponent[0];
    *(pDCM+7) = zComponent[1];
    *(pDCM+8) = zComponent[2];*/


    DCM[0][0] = xComponent[0];
    DCM[0][1] = xComponent[1];
    DCM[0][2] = xComponent[2];
    DCM[1][0] = yComponent[0];
    DCM[1][1] = yComponent[1];
    DCM[1][2] = yComponent[2];
    DCM[2][0] = zComponent[0];
    DCM[2][1] = zComponent[1];
    DCM[2][2] = zComponent[2];

    std::cout << "DCM = " << std::endl << "|" << DCM[0][0] << " " << DCM[0][1] << " " << DCM[0][2] << "|" << std::endl;
    std::cout << "|" << DCM[1][0] << " " << DCM[1][1] << " " << DCM[1][2] << "|" << std::endl;
    std::cout << "|" << DCM[2][0] << " " << DCM[2][1] << " " << DCM[2][2] << "|" << std::endl;

    //find rotation matrix to pitch axes to account for magnetic inclination
    rotationPitch(magneticPitch,&rotationMatrix[0][0]);

    std::cout << "rotationMatrix = " << std::endl << "|" << rotationMatrix[0][0] << " " << rotationMatrix[0][1] << " " << rotationMatrix[0][2] << "|" << std::endl;
    std::cout << "|" << rotationMatrix[1][0] << " " << rotationMatrix[1][1] << " " << rotationMatrix[1][2] << "|" << std::endl;
    std::cout << "|" << rotationMatrix[2][0] << " " << rotationMatrix[2][1] << " " << rotationMatrix[2][2] << "|" << std::endl;

    //rotate DCM
    double DCMrotated[3][3];
    matrixMult3x3by3x3(rotationMatrix,DCM,&DCMrotated[0][0]);
    std::cout << "DCM rotated= " << std::endl << "|" << DCMrotated[0][0] << " " << DCMrotated[0][1] << " " << DCMrotated[0][2] << "|" << std::endl;
    std::cout << "|" << DCMrotated[1][0] << " " << DCMrotated[1][1] << " " << DCMrotated[1][2] << "|" << std::endl;
    std::cout << "|" << DCMrotated[2][0] << " " << DCMrotated[2][1] << " " << DCMrotated[2][2] << "|" << std::endl;

    //extract quaternion from DCM
    std::cout << "pre dcm quaternion = " << quaternion[0] << " " << quaternion[1]
                << " " << quaternion[2] << " " << quaternion[3] << std::endl;
    quaternionFromDCM(DCMrotated, &quaternion[0]);
    std::cout << "aftr dcm quaternion = " << quaternion[0] << " " << quaternion[1]
                << " " << quaternion[2] << " " << quaternion[3] << std::endl;
}
