/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Gland
 * All rights reserved.
 *
 * Authory: Joseph Gland <jgland@umd.edu>
 * File:  packages/control/include/Controller.h
 */

#ifndef RAM_IMU_FUNCTIONS_H_06_30_2007
#define RAM_IMU_FUNCTIONS_H_06_30_2007

#ifdef __cplusplus
extern "C" {
#endif



void quaternionFromIMU(double mag[3],
                       double accel[3],
                       double magneticPitch,
                       double * pQ);

void IMUToVehicleRotationMatrix(double * pRotationMatrix);

// defines for this program only, for real code these are parameters or internal variables
#define CONSTANTmagneticPitchAngle 66
#define PI 3.14159



#ifdef __cplusplus
} // extern "C"
#endif

#endif // RAM_CONTROL_CONTROLLER_H_06_30_2007

