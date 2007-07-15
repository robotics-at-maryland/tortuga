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
namespace ram {
namespace control {
#endif // RAM_MATLAB_CONTROL_TEST

#ifdef __cplusplus
extern "C" {
#endif

// DesiredState = struct('speed',1,
//                       'depth',1,
//                       'quaternion',[1 2 3 4]',
//                       'angularRate',[1 2 3]');
struct DesiredState{
    double speed;
    double depth;
    double quaternion[4];
    double angularRate[3];
};

// MeasuredState = struct('depth',1,
//                        'linearAcceleration',[1 2 3]',
//                        'quaternion',[1 2 3 4]',
//                        'angularRate',[1 2 3]');
struct MeasuredState{
    double depth;
    double linearAcceleration[3];
    double magneticField[3];
    double quaternion[4];
    double angularRate[3];
};

struct ControllerState{
    double angularPGain;
    double angularDGain;
    double inertiaEstimate[3][3];
    double depthPGain;
    double speedPGain;
};

void translationalController(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
                             double dt,
                             double* translationalForces);

void BongWiePDRotationalController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState,
                                   double dt,
                                   double* rotationalTorques);

  //TODO: implement this function
  //  bool isAtDepth();

//TODO: implement this function
  bool doIsOriented(MeasuredState* measuredState,
                DesiredState* desiredState);

double  HackedPDPitchControl(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
			     double hackedPitchGain);

double  HackedPDYawControl(MeasuredState* measuredState,
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
