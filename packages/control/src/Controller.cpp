/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Gland
 * All rights reserved.
 *
 * Authory: Joseph Gland <jgland@umd.edu>
 * File:  packages/control/src/Controller.cpp
 */

#include "Controller.h"
#include "joeMath.h"

/********************************************************************
function translationalController.m is a decoupled controller for vertical and
fore/aft control only.  Note that the fore/aft control is completely open loop

assumes the quaternion is written as

q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]

returns translationalForces, the forces used to translate the vehicle as
written in the vehicle's coord frame.  forces are in newtons
*/
void translationalController(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
                             double dt,
                             double* translationalForces){

}


/************************************************************************
BongWiePDControl(MeasuredState,DesiredState,ControllerState,dt,translationalForces)

function BongWiePDControl.m is an encapsulation of the nonlinear eigenaxis
rotational controller described in Bong Wie's book "Space Vehicle Dynamics
and Control"

assumes the quaternion is written as

q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]

for simplicity assumes desired angular velocity is zero


returns
          - rotationalTorques, the torques used to rotate the vehicle as
            written in the vehicle's coord frame.  torques are in Newton*meters
*/
void rotationalController(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          double dt,
                          double* rotationalTorques){

    //generate proportional controller gain matrix
    double PGain[3][3];
    matrixMult3x3byScalar(controllerState->inertiaEstimate,
                          controllerState->angularPGain,&PGain[0][0]);
    //generate derivative controller gain matrix
    double DGain[3][3];
    matrixMult3x3byScalar(controllerState->inertiaEstimate,
                          controllerState->angularDGain,&DGain[0][0]);

    //compute quaternion error
    double qError[4];
    findErrorQuaternion(desiredState->quaternion,measuredState->quaternion,&qError[0]);

    //angular rate error assumed to be measured angular rate
    double wError[3];
    wError[0]=measuredState->angularRate[0];
    wError[1]=measuredState->angularRate[1];
    wError[2]=measuredState->angularRate[2];

    //final control law
    double controlSignal[3];
    double proportionalSignal[3];
    double differentialSignal[3];
    double gyroscopicSignal[3];
    double wTilde[3][3];
    //proprotional component (yes, we're only taking the first three numbers
    //the quaternion error, the fourth number is redundant)
    matrixMult3x1by3x3(PGain,qError,&proportionalSignal[0]);
    matrixMult3x1byScalar(proportionalSignal,-1,&proportionalSignal[0]);
    //differential component
    matrixMult3x1by3x3(DGain,wError,&differentialSignal[0]);
    matrixMult3x1byScalar(differentialSignal,-1,&differentialSignal[0]);
    //gyroscopic component
    tilde(measuredState->angularRate,&wTilde[0][0]);
    matrixMult3x1by3x3(controllerState->inertiaEstimate,
                        measuredState->angularRate,&gyroscopicSignal[0]);
    matrixMult3x1by3x3(wTilde,gyroscopicSignal,&gyroscopicSignal[0]);
    //sum the components
    matrixAdd3x1and3x1(proportionalSignal,differentialSignal,&controlSignal[0]);
    matrixAdd3x1and3x1(controlSignal,gyroscopicSignal,&controlSignal[0]);
    //save to memory
    *(rotationalTorques)=controlSignal[0];
    *(rotationalTorques+1)=controlSignal[1];
    *(rotationalTorques+2)=controlSignal[2];
}
