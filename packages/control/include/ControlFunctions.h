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

// Project Includes
#include "math/include/Vector2.h"
#include "math/include/Matrix2.h"

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

struct RAM_EXPORT EstimatedState{
    //estimated state for a 2x2 observer controller
    math::Vector2 xHat2Depth;  //<- want to use this, but having trouble
    //double xHat2;
};

struct RAM_EXPORT ControllerState{
    double angularPGain;
    double angularDGain;
    double inertiaEstimate[3][3];
    
    /* DEPTH CONTROL GAINS*/
    //for depth P control
    double depthPGain;
    //for depth observer controller
    
    
    double speedPGain;
    int depthControlType;
    math::Matrix2 depthA;
    math::Vector2 depthB;
    math::Vector2 depthC;
    math::Vector2 depthK;
    math::Vector2 depthL;
    double dt;
    
};

void RAM_EXPORT translationalController(MeasuredState* measuredState,
                                        DesiredState* desiredState,
                                        ControllerState* controllerState,
                                        EstimatedState* estimatedState,
                                        double dt,
                                        double* translationalForces);

void RAM_EXPORT BongWiePDRotationalController(MeasuredState* measuredState,
                                              DesiredState* desiredState,
                                              ControllerState* controllerState,
                                              double dt,
                                              double* rotationalTorques);
                                   
double RAM_EXPORT depthPController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState);
    
double RAM_EXPORT depthObserverController(MeasuredState* measuredState,
                                          DesiredState* desiredState,
                                          ControllerState* controllerState,
                                          EstimatedState* estimatedState,
                                          double dt);
                            
void depthObserver2(MeasuredState* measuredState,
                    DesiredState* desiredState,
                    ControllerState* controllerState,
                    EstimatedState* estimatedState,
                    double dt);

double depthPDController2(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          EstimatedState* estimatedState);
    
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
