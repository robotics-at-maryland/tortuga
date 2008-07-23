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
#define M_PI 3.141592653589793238464
#endif

// Project Includes
#include "control/include/ControlFunctions.h"
#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix3.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector2.h"
#include "math/include/Matrix2.h"
#include "math/include/Vector4.h"
#include "math/include/Matrix4.h"
#include "math/include/MatrixN.h"

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
			     EstimatedState* estimatedState,
                             double dt,
                             double* translationalForces){

    double translationControlSignal[3];

    double depthControlSignal=0;

    /*Select type of Depth Controller
      set depthControlType = (1 for P control) 
                             (2 for observer control for a 2d depth state) 
                             (3 for observer control for a 4d depth state)
                             (4 for observer control with scaling for a 4d depth sate )
                             (5 for pid control)
    Note: 3 and 4 not implemented yet
    */

    switch(controllerState->depthControlType)
    {
	case 1 :
        depthControlSignal=depthPController(measuredState,desiredState,controllerState);
	    break;
	case 2 :
            depthObserver2(measuredState,desiredState,controllerState,estimatedState,dt);
            depthControlSignal=depthPDController2(measuredState,desiredState,controllerState,estimatedState);
	    break;
	case 3 :
           depthControlSignal=depthObserverController4(measuredState,desiredState,controllerState,
                                                      estimatedState,dt);

            break;
	case 4 :
            depthControlSignal=depthObserverController4WithScaling(measuredState,desiredState,
                                                                controllerState,
                                                                estimatedState,dt);
            break;
    case 5 :
            depthControlSignal=depthPIDController(measuredState,desiredState,
                                                                controllerState,
                                                                estimatedState,dt);
            break;

	default :
	    depthControlSignal=depthPController(measuredState,desiredState,controllerState);
	    break;
    }

    
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
    foreAftComponent[1] =
        (controllerState->sidewaysSpeedPGain) * (desiredState->sidewaysSpeed);
    foreAftComponent[2] = 0;


    //combine fore-aft with depth control
    matrixAdd3x1and3x1(translationControlSignal, foreAftComponent, &translationControlSignal[0]);

    //save to memory
    *(translationalForces) = translationControlSignal[0];
    *(translationalForces+1) = translationControlSignal[1];
    *(translationalForces+2) = translationControlSignal[2];
}

/************************************************************************
depthPController(measuredState,desiredState,controllerState)

implements a crappy P controller to regulate depth

Fthrust=-k(x_measured-x_desired)
reads the gain "k" from the config file 

returns a depth control signal intended to be the control force used in the +z axis (inertial coord frame)
*/
double depthPController(MeasuredState* measuredState,
                        DesiredState* desiredState,
                        ControllerState* controllerState){
    double depthError;
    double depthControlSignal;
    double depthPGain=controllerState->depthPGain;
    
    //compute difference
    depthError=measuredState->depth-desiredState->depth;
    //compute control law u=-k(x_meas-x_desire)
    depthControlSignal = (-1)*depthPGain*depthError;
    return depthControlSignal;
}


/************************************************************************
depthObserver2(measuredState,desiredState,controllerState,estimatedState,dt)

Modifies the estimate from State Estimate

implements a Luenberger state observer for depth

xHatDot=A*xHat+B*u+L(y-C*xHat)

where xHat=[depthHat depthDotHat]'

Only Changes estimatedState.xHat2Depth
*/
void depthObserver2(MeasuredState* measuredState,
                    DesiredState* desiredState,
                    ControllerState* controllerState,
                    EstimatedState* estimatedState,
                    double dt)
{
	Vector2 xHat2 =  estimatedState->xHat2Depth;
	double yHat = controllerState->depthC.dotProduct(xHat2);
    //std::cout << "dt:" << dt << " " << controllerState->dtMin << " " << controllerState->dtMax << std::endl;
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }
	
	// xHatDot = (A-B*K)*xHat+L*(y-yHat);
	
	Vector2 xHatDot = (controllerState->depthA)*xHat2 +
	 (controllerState->depthB).dotProduct(-controllerState->depthK)*xHat2 +
	 (controllerState->depthL)*(measuredState->depth - yHat);

	xHat2 = xHat2 + dt*xHatDot;

	estimatedState->xHat2Depth = xHat2;
}


/************************************************************************
depthPDController2(measuredState,desiredState,controllerState,estimatedState)

implements a PD Controller to regulate depth

U = -K*xHat
Fthrust = U

returns a depth control signal intended to be the control force used in the +z axis (inertial coord frame)
*/
double depthPDController2(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          EstimatedState* estimatedState)
{

	Vector2 xHat2 = estimatedState->xHat2Depth;
	Vector2 xDesired2 = Vector2(desiredState->depth,0);
	double depthControlSignal = -controllerState->depthK.dotProduct(xHat2-xDesired2);
	return depthControlSignal;

}

/************************************************************************/
double depthPIDController(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          EstimatedState* estimatedState,
                          double dt)
{
    //std::cout << "dt:" << dt << " " << controllerState->dtMin << " " << controllerState->dtMax << std::endl;
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }

    double error = measuredState->depth-desiredState->depth;
    double errorDot = (measuredState->depth-controllerState->depthPrevX)/dt;
    double errorInt = controllerState->depthSumError+error*dt;
    controllerState->depthSumError = errorInt;
    controllerState->depthPrevX = measuredState->depth;

    double depthControlSignal = -(controllerState->depthKp)*error-(controllerState->depthKd)        
                                *errorDot-(controllerState->depthKi)*errorInt;
	return depthControlSignal;

}

/************************************************************************/



/************************************************************************
depthObserverController4(measuredState,desiredState,controllerState,estimatedState)
implements an Observer Controller with Integral Augmentation

xHat4_dot = A_c*xHat4 + B_c*(y-xd)

xHat4 = xHat4 + xHat4_dot*dt

Fthrust = C_c*xHat4

returns a depth control signal intended to be the control force used in the +z axis (inertial coord frame)
*/
double depthObserverController4(MeasuredState* measuredState,
                                DesiredState* desiredState,
                                ControllerState* controllerState,
                                EstimatedState* estimatedState,
                                double dt)
{
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }

    Vector4 xHat4 = estimatedState->xHat4Depth;
    Vector4 xHat4Dot = controllerState->depthA4*xHat4 +
                       controllerState->depthB4*
                       (measuredState->depth - desiredState->depth);

    xHat4 = xHat4 + xHat4Dot*dt;
    estimatedState->xHat4Depth = xHat4;

    double depthControlSignal = controllerState->depthC4.dotProduct(xHat4);
	
	return depthControlSignal;
}

/************************************************************************
depthObserverController4WithScaling(measuredState,desiredState,controllerState,estimatedState)
implements an Observer Controller with Integral Augmentation

xHat4_dot = A_c*xHat4 + B_c*(y-xd)

xHat4 = xHat4 + xHat4_dot*dt

Fthrust = C_c*xHat4

returns a depth control signal intended to be the control force used in the +z axis (inertial coord frame)
*/
double depthObserverController4WithScaling(MeasuredState* measuredState,
                                DesiredState* desiredState,
                                ControllerState* controllerState,
                                EstimatedState* estimatedState,
                                double dt)
{
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }

    Vector4 xHat4 = estimatedState->xHat4Depth;
    Vector4 xHat4Dot = controllerState->depthA4*xHat4 +
                       controllerState->depthB4*
                       (measuredState->depth - desiredState->depth);

    xHat4 = xHat4 + xHat4Dot*dt;
    estimatedState->xHat4Depth = xHat4;

    double depthControlSignal = controllerState->depthC4.dotProduct(xHat4);
    
	double error = (measuredState->depth - desiredState->depth)/desiredState->depth;
	double cutoff = 0.1;
	if (error > -1*cutoff && error < cutoff && dt != 1)
	    depthControlSignal*= 0.3;
	
	return depthControlSignal;
}

/************************************************************************
depthObserverController4Discrete(measuredState,desiredState,controllerState,estimatedState)
implements an Observer Controller with Integral Augmentation

xHat4 = A_c*xHat4 + B_c*(y-xd)

Fthrust = C_c*xHat4

returns a depth control signal intended to be the control force used in the +z axis (inertial coord frame)
*/
double depthObserverController4Discrete(MeasuredState* measuredState,
                                        DesiredState* desiredState,
                                        ControllerState* controllerState,
                                        EstimatedState* estimatedState,
                                        double dt)
{
    if(controllerState->dtMin > dt)
    {
        dt = controllerState->dtMin;
    }
    if(controllerState->dtMax < dt)
    {
        dt = controllerState->dtMax;
    }

    Vector4 xHat4 = estimatedState->xHat4Depth;
    xHat4 = controllerState->depthA4*xHat4 + controllerState->depthB4*
            (measuredState->depth - desiredState->depth);
    estimatedState->xHat4Depth = xHat4;

    double depthControlSignal = controllerState->depthC4.dotProduct(xHat4);
		
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
                                   double* rotationalTorques)
{
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }

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
    // fourth (scalar) term is used above to correct a sign flipping error
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
AdaptiveRotationalController(MeasuredState,DesiredState,ControllerState,dt,translationalForces)

AdaptiveRotationalController is a modified version of the adaptive spacecraft controller described in:
Egeland, O., and Godhavn, J.-M., "Passivity-based adaptive attitude control of a rigid spacecraft", IEEE Transactions on Automatic Control, 39(4), pp. 842-845, 1994.
The controller has been modified by Joseph Gland to adaptively learn the following parameters: inertia, drag, buoyancy

assumes the quaternion is written as

q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]



returns
          - rotationalTorques, the torques used to rotate the vehicle as
            written in the vehicle's coord frame.  torques are in Newton*meters
*/
void AdaptiveRotationalController(MeasuredState* measuredState,
                                   DesiredState* desiredState,
                                   ControllerState* controllerState,
                                   double dt,
                                   double* rotationalTorques)
{
  //be extra safe about dt values
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }

	//format quaternion desired for OGRE
	Quaternion qd(desiredState->quaternion);
	//format actual quaternion for OGRE
	Quaternion q(measuredState->quaternion);
	//format angular rate desired for OGRE
	Vector3 wd(desiredState->angularRate);
	//derivative of angular rate desired
	Vector3 dwd(0,0,0);
	//format angular rate for OGRE
	Vector3 w(measuredState->angularRate);
	//compute derivative of quaternion desired
	Quaternion dqd = qd.derivative(wd);
	
	//compute error quaternion
	//WARNING!!! I had to "transpose" the quaternion order here.  
	//Is this a problem with the errorQuaternion function (ie backwards)?
	//	Quaternion qc_tilde = q.errorQuaternion(qd);
	Quaternion qc_tilde = qd.errorQuaternion(q);
	//	std::cout << "qc = " << qc_tilde.x << " " << qc_tilde.y << " " << qc_tilde.z << " " << qc_tilde.w << std::endl;

	//compute rotation matrix
	Matrix3 RotMatc_tilde;
	qc_tilde.ToRotationMatrix(RotMatc_tilde);

	//extract vector portion of qc_tilde
	Vector3 epsilon_c_tilde(qc_tilde.x, qc_tilde.y, qc_tilde.z);

	
	//compute composite error metrics
	Vector3 w_r = RotMatc_tilde*wd-(controllerState->adaptCtrlRotLambda)*epsilon_c_tilde;
	Vector3 shat = w-w_r;

	//compute angular rate error
	Vector3 wc_tilde = w -RotMatc_tilde*wd;

	//	double rotationalTorques[3];
	rotationalTorques[0] = wc_tilde[0];
	rotationalTorques[1] = wc_tilde[1];
	rotationalTorques[2] = wc_tilde[2];
}

/************************************************************************
*/
/*
Matrix3 S(Vector3 e)
{
    return Matrix3(0, -1*e.z, e.y, e.z, 0, -1*e.x, -1*e.y, e.x, 0);
}

MatrixN Q(Quaternion q)
{
    Vector3 e = Vector3(q.x,q.y,q.z);
    Matrix3 q1 = q.w*Matrix3::IDENTITY + S(e);
    MatrixN qOut = MatrixN(4,3);
    qOut[0][0] = q1[0][0];
    qOut[0][1] = q1[0][1];
    qOut[0][2] = q1[0][2];
    qOut[1][0] = q1[1][0];
    qOut[1][1] = q1[1][1];
    qOut[1][2] = q1[1][2];
    qOut[2][0] = q1[2][0];
    qOut[2][1] = q1[2][1];
    qOut[2][2] = q1[2][2];    
    qOut[3][0] = -1*e.x;
    qOut[3][1] = -1*e.y;
    qOut[3][2] = -1*e.z;
    return qOut;
}

Quaternion QuaternionProduct(Quaternion q1, Quaternion q2)
{
    Matrix3 temp1 = q2.w*Matrix3::IDENTITY - S(Vector3(q2.x,q2.y,q2.z));
    
    Matrix4 temp2 = temp1;
    temp2[0][3] = -1*q2.x;
    temp2[1][3] = -1*q2.y;
    temp2[2][3] = -1*q2.z;
    temp2[3][0] = -1*q2.x;
    temp2[3][1] = -1*q2.y;
    temp2[3][2] = -1*q2.z;
    temp2[3][3] = q2.w;
    
    Vector4 q1Temp = Vector4(q1.x,q1.y,q1.z,q1.w);
    Vector4 qtilde = temp2*q1Temp;
    
    Quaternion qOut = Quaternion(qtilde.x,qtilde.y,qtilde.z,qtilde.w);
    double norm = qOut.Norm();
    norm = 1 / norm;
    qOut = qOut * norm;
    return qOut;
}

Matrix3 R(Quaternion q)
{
    Vector3 e = Vector3(q.x,q.y,q.z);
    double temp = q.w * q.w - e.dotProduct(e);
    
    Matrix3 tempMat;
    tempMat[0][0] = e.x*e.x;
    tempMat[0][1] = e.y*e.y;
    tempMat[0][2] = e.z*e.z;
    tempMat[1][0] = e.x*e.x;
    tempMat[1][1] = e.y*e.y;
    tempMat[1][2] = e.z*e.z;
    tempMat[2][0] = e.x*e.x;
    tempMat[2][1] = e.y*e.y;
    tempMat[2][2] = e.z*e.z;
    Matrix3 out = temp*Matrix3::IDENTITY + 2*tempMat - 2*q.w*S(e);
    return out;
}

Matrix3 Q1(Quaternion q)
{
    Vector3 e = Vector3(q.x,q.y,q.z);
    return q.w * Matrix3::IDENTITY + S(e);
}

void RotationalController(MeasuredState* measuredState,
                          DesiredState* desiredState,
                          ControllerState* controllerState,
                          double dt,
                          double* rotationalTorques)
{
    if(dt < controllerState->dtMin)
    {
        dt = controllerState->dtMin;
    }
    if(dt > controllerState->dtMax)
    {
        dt = controllerState->dtMax;
    }
        
    Vector3 desiredAngularRate = Vector3(desiredState->angularRate);
    
    Quaternion desiredQuaternion = desiredState->quaternion;
    
    Vector3 dDesiredAngularRate = Vector3(0,0,0);
    
    desiredAngularRate = desiredAngularRate + dt*dDesiredAngularRate;
    
    MatrixN dq_d = (0.5)*Q(desiredQuaternion)*desiredAngularRate;
    
    Quaternion dDesiredQuaternion = Quaternion(dq_d[0][0], dq_d[1][0], dq_d[2][0], dq_d[3][0]);
    
    desiredQuaternion = desiredQuaternion + dt*dDesiredQuaternion;
    
    desiredState->angularRate[0] = desiredAngularRate.x;
    desiredState->angularRate[1] = desiredAngularRate.y;
    desiredState->angularRate[2] = desiredAngularRate.z;
    desiredState->quaternion[0] = desiredQuaternion.x;
    desiredState->quaternion[1] = desiredQuaternion.y;
    desiredState->quaternion[2] = desiredQuaternion.z;
    desiredState->quaternion[3] = desiredQuaternion.w;
    
    // Compute attitude error qc_tilde
    Quaternion qc_tilde = QuaternionProduct(measuredState->quaternion,desiredQuaternion);
    
    // Compute composite error metric s
    Vector3 q3 = Vector3(qc_tilde.x,qc_tilde.y,qc_tilde.z);
    
    Matrix3 rotQc = R(qc_tilde);
    Vector3 w_r = rotQc*desiredAngularRate - controllerState->lamda*q3;
    
    Vector3 measuredAngularRate = Vector3(measuredState->angularRate);
    Vector3 estimatedS = measuredAngularRate - w_r;
    
    // Compute anglar rate error wc_tilde
    Vector3 wc_tilde = measuredAngularRate - rotQc*desiredAngularRate;
    
    Vector3 dw_r = rotQc*dDesiredAngularRate - S(wc_tilde)*rotQc*desiredAngularRate
                    - lambda*Q1(qc_tilde)*wc_tilde;
                    
    Vector3 controlSignal = -1*controllerState->Kd * estimatedS + controllerState->H
                                *dw_r;
    
    *(rotationalTorques)=controlSignal.x;
    *(rotationalTorques+1)=controlSignal.y;
    *(rotationalTorques+2)=controlSignal.z;
}
*/

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
			   double hackedPitchGain)
{
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

