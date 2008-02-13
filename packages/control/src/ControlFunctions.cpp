/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Gland
 * All rights reserved.
 *
 * Authory: Joseph Gland <jgland@umd.edu>
 * File:  packages/control/src/ControlFunctions.cpp
 */

//STD Includes
#include <cmath>
#include <cstdio>
#include <iostream>

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif

// Project Includes
#include "control/include/ControlFunctions.h"
#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

#ifndef RAM_MATLAB_CONTROL_TEST
namespace ram {
namespace control {
#endif // RAM_MATLAB_CONTROL_TEST

// Brings ram::math Helper functions into scope
using namespace ram::math;

using namespace std;
    
/********************************************************************
function translationalController.m is a decoupled controller 

all translational forces and control laws should be implemented here

at the moment, only depth (global coordinates) and fore/aft (relative
coordinates) motions are currently implemented

Note that the fore/aft control is completely open loop

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

    double translationControlSignal[3];

    /*
    //depth control
    //depth component first calculated in inertial frame for a single axis
    //first find depth error (proportional component)
    double depthError;
    
    // Flipped based on testing
    depthError = (measuredState->depth) - (desiredState->depth);
//    printf("D: %7.4f DE %7.4f\n", measuredState->depth, depthError);
//    depthError = (desiredState->depth)-(measuredState->depth);

    //second implement depth error in single axis control law
    double depthControlSignal;
    depthControlSignal = (-1)*(controllerState->depthPGain)*depthError;
    */
    
    //do P control for now
    double depthControlSignal;
    depthControlSignal=depthPController(measuredState->depth,desiredState->depth,controllerState);
    
    //now put single axis control signal in a proper inertial frame
    double depthComponent[3];
    depthComponent[0]=0;
    depthComponent[1]=0;
    depthComponent[2]=-depthControlSignal;

    //now rotate depth control component to the vehicle's coordinate frame
    double rotationMatrix[3][3];
    rotationMatrixFromQuaternion(measuredState->quaternion,&rotationMatrix[0][0]);
    matrixMult3x1by3x3(rotationMatrix,depthComponent,&translationControlSignal[0]);

    /*
    // Quaternion is bogus doing it manually
    translationControlSignal[0] = 0;
    translationControlSignal[1] = 0;
    translationControlSignal[2] = -depthControlSignal;
    */
            
    //fore-aft control (open loop, not really control) done in vehicle coordinates
    double foreAftComponent[3];
    foreAftComponent[0] = (controllerState->speedPGain)*(desiredState->speed);
    foreAftComponent[1] = 0;
    foreAftComponent[2] = 0;


    //combine fore-aft with depth control
    matrixAdd3x1and3x1(translationControlSignal, foreAftComponent, &translationControlSignal[0]);

    //save to memory
    *(translationalForces) = translationControlSignal[0];
    *(translationalForces+1) = translationControlSignal[1];
    *(translationalForces+2) = translationControlSignal[2];
}

/************************************************************************
depthPController(measuredDepth,desiredDepth)

implements a crappy P controller to regulate depth

Fthrust=-k(x_measured-x_desired)
reads the gain "k" from the config file 

returns a depth control signal intended to be the control force used in the +z axis (inertial coord frame)
*/
double depthPController(double measuredDepth,
                        double desiredDepth,
                        ControllerState* controllerState){
    double depthError;
    double depthControlSignal;
    double depthPGain=controllerState->depthPGain;
    
    //compute difference
    depthError=measuredDepth-desiredDepth;
    //compute control law u=-k(x_meas-x_desire)
    depthControlSignal = (-1)*depthPGain*depthError;
    return depthControlSignal;
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
void BongWiePDRotationalController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState,
                                   double dt,
                                   double* rotationalTorques){

    // Generate proportional controller gain matrix
    double PGain[3][3];
    matrixMult3x3byScalar(controllerState->inertiaEstimate,
                          controllerState->angularPGain,&PGain[0][0]);

    // Generate derivative controller gain matrix
    double DGain[3][3];
    matrixMult3x3byScalar(controllerState->inertiaEstimate,
                          controllerState->angularDGain,&DGain[0][0]);

    // Compute quaternion error
    double qError[4];
    findErrorQuaternion(desiredState->quaternion,
                        measuredState->quaternion,&qError[0]);
    // Use the scalar term of error quaternion to eliminate sign flipping
    double scalarTermSign = qError[3] / fabs(qError[3]);
    matrixMult4x1byScalar(&qError[0], scalarTermSign, &qError[0]);
    
    // Angular rate error assumed to be measured angular rate
    double wError[3];
    wError[0]=measuredState->angularRate[0];
    wError[1]=measuredState->angularRate[1];
    wError[2]=measuredState->angularRate[2];

    // Final control law
    double controlSignal[3];
    double proportionalSignal[3];
    double differentialSignal[3];
    double gyroscopicSignal[3];
    double wTilde[3][3];
    
    // Proprotional component, only the first three numbers are used here, the
    // forth (scalar) term is used above to correct a sign flipping error
    matrixMult3x1by3x3(PGain,qError,&proportionalSignal[0]);
    matrixMult3x1byScalar(proportionalSignal,-1,&proportionalSignal[0]);
    // Differential component
    matrixMult3x1by3x3(DGain,wError,&differentialSignal[0]);
    matrixMult3x1byScalar(differentialSignal,-1,&differentialSignal[0]);
    // Gyroscopic component
    tilde(measuredState->angularRate,&wTilde[0][0]);
    matrixMult3x1by3x3(controllerState->inertiaEstimate,
                        measuredState->angularRate,&gyroscopicSignal[0]);
    matrixMult3x1by3x3(wTilde,gyroscopicSignal,&gyroscopicSignal[0]);
    // Sum the components
    matrixAdd3x1and3x1(proportionalSignal,differentialSignal,&controlSignal[0]);
    matrixAdd3x1and3x1(controlSignal,gyroscopicSignal,&controlSignal[0]);
    //save to memory
    *(rotationalTorques)=controlSignal[0];
    *(rotationalTorques+1)=controlSignal[1];
    *(rotationalTorques+2)=controlSignal[2];
}


/************************************************************************
HackedPDYawControl

Incredibly dirty implementation of a SISO PD controller for yaw control
only.  The controller uses the rotated and filtered values from the 
accelerometer, along with an assumption that the vehicle isn't rolling, to 
create an estimate of the yaw.
*/
  double HackedPDYawControl(MeasuredState* measuredState,
                        DesiredState* desiredState,
			   ControllerState* controllerState,
			   double hackedYawGain){
    /*  double mag1=measuredState->magneticField[0];
  double mag2=measuredState->magneticField[1];
  /////////////start from here
  double psiMeas=atan2(accel3,accel1);
  double psiDes=-1.5708;
  double psiError = thetaMeas-thetaDes;

  // For conditions when pitch is so large the gravity vector flips 
  // (pitch > 90)
  if (thetaMeas>3.14){
    thetaError=thetaError-2*M_PI;
  }

//  printf("E: %5.3f A1: %5.3f A2: %5.3f\n", thetaError, accel1, accel3);
  //I need a minus sign in front of the gain, right...?
  //this is P control for pitch (no you don't)
//  double pitchTorque= hackedPitchGain*thetaError;

  //a better line to run:
  //this is PD control for pitch
   double pitchTorque= hackedPitchGain*thetaError
       + (-1)*controllerState->angularDGain*(measuredState->angularRate[1]-0);
       return pitchTorque;  */
      return 0;
}



/************************************************************************
HackedPDPitchControl

Incredibly dirty implementation of a SISO PD controller for pitch control
only.  The controller uses the rotated and filtered values from the 
accelerometer, along with an assumption that the vehicle isn't rolling, to 
create an estimate of the pitch.
*/

double HackedPDPitchControl(MeasuredState* measuredState,
                           DesiredState* desiredState,
			   ControllerState* controllerState,
			   double hackedPitchGain){
  double accel1=measuredState->linearAcceleration[0];
  double accel3=measuredState->linearAcceleration[2];
  double thetaMeas=atan2(accel3,accel1);
  double thetaDes=-1.5708;
  double thetaError = thetaMeas-thetaDes;

  // For conditions when pitch is so large the gravity vector flips 
  // (pitch > 90)
  if (thetaMeas>3.14){
    thetaError=thetaError-2*M_PI;
  }

//  printf("E: %5.3f A1: %5.3f A2: %5.3f\n", thetaError, accel1, accel3);
  //I need a minus sign in front of the gain, right...?
  //this is P control for pitch (no you don't)
//  double pitchTorque= hackedPitchGain*thetaError;

  //a better line to run:
  //this is PD control for pitch
   double pitchTorque= hackedPitchGain*thetaError
       + (-1)*controllerState->angularDGain*(measuredState->angularRate[1]-0);
  return pitchTorque;  
}



  //TODO: implement this function
  /*  bool isAtDepth(){

  }*/

  //TODO: implement this function
bool doIsOriented(MeasuredState* measuredState,
                  DesiredState* desiredState,
                  double threshold){
  bool amIOriented;
  double qError[4];
  findErrorQuaternion(desiredState->quaternion,
		      measuredState->quaternion,
                      &qError[0]);
  
  // This does the sqrt of the sum of the squares for the first three elements
  Vector3 tmp(qError);
//  cout << "Q: " << tmp << " 4th: " <<  qError[3] << " Mag: " << tmp.length()
//       << "T: " << threshold << endl;
  if(tmp.length() > threshold) {
    amIOriented = false;
  } else {
    amIOriented = true;
  }
  return amIOriented;
}


/************************************************************************
HackedPDRotationalController(MeasuredState,DesiredState,ControllerState,dt,rotationalTorques)

function HackedPDRotationalController is a SISO yaw controller.  it discards everything I know about coupled dynamics and control as a dirty hack to get something working at the competition.  I am shamed to write this function.

This function exclusively controls the yaw axis of the vehicle.  It assumes that pitch and roll are controlled passively through the stabilization of the sub.  This controller does NOT account for any roll or pitch whatsoever.

*/
/*
void HackedPDRotationalController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState,
                                   double dt,
                                   double* rotationalTorques){
    
  //gather bullshit data from the IMU

  //pester Joe about getting this later
  //grab m vector from IMU
  double mag[3];
  //grab angular rate from IMU
  double omega;//assume only interested in yaw
  omega=measuredState->angularRate[2];

  double yawDesired=0;//get Joe to help implement this
  
  //assume the vehicle is perfectly upright
  mag[0] = mag[0];
  mag[1] = mag[1];
  mag[2] = 0;//loose the vertical component of the magnetic field

  //now normalize the field
  normalize3x1(mag);

  //"calculate" yaw
  double yawMeasured;
  yawMeasured = atan2(mag[1],mag[0]);

  //calculate yaw error
  double yawError;
  if yaw < 0                yaw = yaw+2*pi;
  if yaw > 2*M_PI           yaw = yaw - 2*M_PI;
  if yawDesired < 0         yaw = yaw+2*M_PI;
  if yawDesired > 2*M_PI;   yaw = yaw-2*M_PI;

  if yaw-yawDesired > M_PI      yaw = yaw-2*M_PI;
  if yawDesired-yaw > M_pi     yaw = yaw+2*M_PI;


  //control law
  double yawTorque;
  yawTorque=-(controllerState->angularPGain)*(yawDesired-yawMeasured)
    -(controllerState->angularDGain)*(0-omega);

  //save to memory
    *(rotationalTorques)=0;
    *(rotationalTorques+1)=0;
    *(rotationalTorques+2)=yawTorque;
}

*/

#ifndef RAM_MATLAB_CONTROL_TEST
} // namespace control
} // namespace ram
#endif // RAM_MATLAB_CONTROL_TEST

