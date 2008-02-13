/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Gland
 * All rights reserved.
 *
 * Authory: Joseph Gland <jgland@umd.edu>
 * File:  packages/control/include/ControlFunctions.h
 */

#ifndef RAM_CONTROL_CONTROLFUNCTIONS_H_06_30_2007
#define RAM_CONTROL_CONTROLFUNCTIONS_H_06_30_2007

#ifndef RAM_MATLAB_CONTROL_TEST

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {
#else // RAM_MATLAB_CONTROL_TEST
#define RAM_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

// DesiredState = struct('speed',1,
//                       'depth',1,
//                       'quaternion',[1 2 3 4]',
//                       'angularRate',[1 2 3]');
struct RAM_EXPORT DesiredState{
    double speed;
    double depth;
    double quaternion[4];
    double angularRate[3];
};

// MeasuredState = struct('depth',1,
//                        'linearAcceleration',[1 2 3]',
//                        'quaternion',[1 2 3 4]',
//                        'angularRate',[1 2 3]');
struct RAM_EXPORT MeasuredState{
    double depth;
    double linearAcceleration[3];
    double magneticField[3];
    double quaternion[4];
    double angularRate[3];
};

struct RAM_EXPORT ControllerState{
    double angularPGain;
    double angularDGain;
    double inertiaEstimate[3][3];
    double depthPGain;
    double speedPGain;
};

void RAM_EXPORT translationalController(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
                             double dt,
                             double* translationalForces);

void RAM_EXPORT BongWiePDRotationalController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState,
                                   double dt,
                                   double* rotationalTorques);
                                   
double RAM_EXPORT depthPController(double measuredDepth,
                            double desiredDepth,
                            ControllerState* controllerState);

  //TODO: implement this function
  //  bool isAtDepth();

//TODO: implement this function
  bool RAM_EXPORT doIsOriented(MeasuredState* measuredState,
                    DesiredState* desiredState,
                    double threshold);

double RAM_EXPORT HackedPDPitchControl(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
			     double hackedPitchGain);

double RAM_EXPORT HackedPDYawControl(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
			     double hackedYawGain);


  /*void HackedPDRotationalController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState,
                                   double dt,
                                   double* rotationalTorques);*/


#ifdef __cplusplus
} // extern "C"
#endif

#ifndef RAM_MATLAB_CONTROL_TEST
} // namespace control
} // namespace ram
#endif // RAM_MATLAB_CONTROL_TEST
    
#endif // RAM_CONTROL_CONTROLFUNCTIONS_H_06_30_2007
