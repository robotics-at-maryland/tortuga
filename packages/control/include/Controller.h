/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Gland
 * All rights reserved.
 *
 * Authory: Joseph Gland <jgland@umd.edu>
 * File:  packages/control/include/Controller.h
 */

#ifndef RAM_CONTROL_CONTROLLER_H_06_30_2007
#define RAM_CONTROL_CONTROLLER_H_06_30_2007

#ifdef __cplusplus
extern "C" {
#endif

// DesiredState = struct('speed',1,
//                       'depth',1,
//                       'quaternion',[1 2 3 4]',
//                       'angularRate',[1 2 3]');
typedef struct
{
    double speed;
    double depth;
    double quaternion[4];
    double angularRate[3];
} DesiredState;

// MeasuredState = struct('depth',1,
//                        'linearAcceleration',[1 2 3]',
//                        'quaternion',[1 2 3 4]',
//                        'angularRate',[1 2 3]');
typedef struct
{
    double depth;
    double linearAcceleration[3];
    double quaternion[4];
    double angularRate[3];
} MeasuredState;


typedef struct
{
    double angularPGain;
    double angularDGain;
    double inertiaEstimate[3][3];
    double depthPGain;
    double speedPGain;
} ControllerState;

void rotationalController(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          double dt,
                          double* rotationalTorques);

void translationalController(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
                             double dt,
                             double* translationalForces);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // RAM_CONTROL_CONTROLLER_H_06_30_2007
