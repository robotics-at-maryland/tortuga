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
#include "math/include/Vector4.h"
#include "math/include/Matrix4.h"
#include "math/include/MatrixN.h"

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
    double sidewaysSpeed;
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
    math::Vector4 xHat4Depth;
};

struct RAM_EXPORT ControllerState{

  /* ROTATIONAL CONTROL GAINS */
  //for nonlinear PD control
    double angularPGain;
    double angularDGain;
    double inertiaEstimate[3][3];
	//for nonlinear adaptive controller
	double adaptCtrlRotK;//controller gain
	double adaptCtrlRotLambda;//replacement model pole
	double adaptCtrlRotGamma;//adaptation gain
	math::MatrixN adaptCtrlParams;//parameter estimate vector
    
    /* DEPTH CONTROL GAINS*/
    //for depth P control
    double depthPGain;
    double dtMin;
    double dtMax;
    //for depth PID control
    double depthKi;
    double depthKp;
    double depthKd;
    double depthSumError;
    double depthPrevX;


    //for depth observer controller
    
    double speedPGain;
    double sidewaysSpeedPGain;
    int depthControlType;
    math::Matrix2 depthA;
    math::Vector2 depthB;
    math::Vector2 depthC;
    math::Vector2 depthK;
    math::Vector2 depthL;

    math::Matrix4 depthA4;
    math::Vector4 depthB4;
    math::Vector4 depthC4;
    
};

void RAM_EXPORT translationalController(MeasuredState* measuredState,
                                        DesiredState* desiredState,
                                        ControllerState* controllerState,
                                        EstimatedState* estimatedState,
                                        double dt,
                                        double* translationalForces);

void RAM_EXPORT rotationalPDController(MeasuredState* measuredState,
                                              DesiredState* desiredState,
                                              ControllerState* controllerState,
                                              double dt,
                                              double* rotationalTorques);

void RAM_EXPORT adaptiveRotationalController(MeasuredState* measuredState,
                                              DesiredState* desiredState,
                                              ControllerState* controllerState,
                                              double dt,
                                              double* rotationalTorques);
                                   
double RAM_EXPORT depthPController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState);
                            
void RAM_EXPORT depthObserver2(MeasuredState* measuredState,
                               DesiredState* desiredState,
                               ControllerState* controllerState,
                               EstimatedState* estimatedState,
                               double dt);

double RAM_EXPORT depthPDController2(MeasuredState* measuredState,
                                     DesiredState* desiredState,
                                     ControllerState* controllerState,
                                     EstimatedState* estimatedState);

double RAM_EXPORT depthPIDController(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          EstimatedState* estimatedState,
                          double dt);

double RAM_EXPORT depthObserverController4(MeasuredState* measuredState,
                                          DesiredState* desiredState,
                                          ControllerState* controllerState,
                                          EstimatedState* estimatedState,
                                          double dt);
										  
double depthObserverController4WithScaling(MeasuredState* measuredState,
                                DesiredState* desiredState,
                                ControllerState* controllerState,
                                EstimatedState* estimatedState,
                                double dt);

double depthObserverController4Discrete(MeasuredState* measuredState,
                    DesiredState* desiredState,
                    ControllerState* controllerState,
                    EstimatedState* estimatedState,
                    double dt);
    
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
