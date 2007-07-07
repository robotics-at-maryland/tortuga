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

/*******************************************************************************
generates a rotation matrix to convert from IMU coordinate frame to vehicle
coordinate frame

all magic numbers generated from Matlab
{vehicle frame} = rotationYaw(pi/2)*rotationRoll(pi/2)*{IMU term}
*/
void IMUToVehicleRotationMatrix(double * pRotationMatrix){
    *(pRotationMatrix) = 0;
    *(pRotationMatrix+1) = 0;
    *(pRotationMatrix+2) = 1;

    *(pRotationMatrix+3) = -1;
    *(pRotationMatrix+4) = 0;
    *(pRotationMatrix+5) = 0;

    *(pRotationMatrix+6) = 0;
    *(pRotationMatrix+7) = -1;
    *(pRotationMatrix+8) = 0;
}



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

    //find zComponent
    crossProduct3x1by3x1(xComponent,yComponent,zComponent);
    //normalize just for shits and giggles
    normalize3x1(zComponent);

    //place x, y, and z components in the direction cosine matrix
    DCM[0][0] = xComponent[0];
    DCM[0][1] = xComponent[1];
    DCM[0][2] = xComponent[2];
    DCM[1][0] = yComponent[0];
    DCM[1][1] = yComponent[1];
    DCM[1][2] = yComponent[2];
    DCM[2][0] = zComponent[0];
    DCM[2][1] = zComponent[1];
    DCM[2][2] = zComponent[2];

    //find rotation matrix to pitch axes to account for magnetic inclination
    rotationPitch(magneticPitch,&rotationMatrix[0][0]);

    //rotate DCM
    double DCMrotated[3][3];
    matrixMult3x3by3x3(rotationMatrix,DCM,&DCMrotated[0][0]);

    //extract quaternion from DCM
    quaternionFromDCM(DCMrotated, &quaternion[0]);
}
