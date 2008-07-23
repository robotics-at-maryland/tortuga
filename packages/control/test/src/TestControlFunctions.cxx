/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Authory: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControlFunctions.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/include/ControlFunctions.h"

using namespace ram;

TEST(BongWiePDRotationalController)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    
    // Quat = 0,0,0,1 and everything else zero
  

	control::DesiredState desired;
	desired.speed=0;
	desired.sidewaysSpeed=0;
	desired.depth=0;
	desired.quaternion[0]=0;
	desired.quaternion[1]=0;
	desired.quaternion[2]=0;
	desired.quaternion[3]=1;
	desired.angularRate[0]=0;
	desired.angularRate[1]=0;
	desired.angularRate[2]=0;
										   
	control::MeasuredState measured;
	measured.depth=0;
	measured.linearAcceleration[0]=0;
	measured.linearAcceleration[1]=0;
	measured.linearAcceleration[2]=0;
	measured.magneticField[0]=0;
	measured.magneticField[1]=0;
	measured.magneticField[2]=0;
	measured.quaternion[0]=0;
	measured.quaternion[1]=0;
	measured.quaternion[2]=0.2756;
	measured.quaternion[3]=0.9613;
	measured.angularRate[0]=0;
	measured.angularRate[1]=0;
	measured.angularRate[2]=0;

	control::ControllerState state;
	state.angularPGain=10;
	state.angularDGain=1;
	state.inertiaEstimate[0][0]=0.201;
	state.inertiaEstimate[0][1]=0;
	state.inertiaEstimate[0][2]=0;
	state.inertiaEstimate[1][0]=0;
	state.inertiaEstimate[1][1]=1.288;
	state.inertiaEstimate[1][2]=0;
	state.inertiaEstimate[2][0]=0;
	state.inertiaEstimate[2][1]=0;
	state.inertiaEstimate[2][2]=1.288;
	state.depthPGain=0;
	state.speedPGain=0;


    // Testing Yaw Control
    double exp_rotTorques[3] = {0, 0, -3.5497};
    double act_rotTorques[3] = {0};
    control::BongWiePDRotationalController(&measured, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques, act_rotTorques, 3, 0.0001);


    // Testing pitch Control
    double exp_rotTorques2[3] = {0, -2.7872, 0};
    control::MeasuredState measured2 = {0, {0}, {0}, 
                                       {0, 0.2164, 0, 0.9763},
                                       {0}};
    control::BongWiePDRotationalController(&measured2, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques2, act_rotTorques, 3, 0.0001);

    // Testing Roll Control
    double exp_rotTorques3[3] = {0.7692, 0, 0};
    control::MeasuredState measured3 = {0, {0}, {0}, 
                                        {-0.3827, 0, 0, 0.9239},
                                       {0}};
    control::BongWiePDRotationalController(&measured3, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques3, act_rotTorques, 3, 0.0001);
}

TEST(AdaptiveRotationalController)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)

	control::DesiredState desired;
	/*	desired.speed=0;
	desired.sidewaysSpeed=0;
	desired.depth=0;*/
	desired.quaternion[0]=0;
	desired.quaternion[1]=0;
	desired.quaternion[2]=0;
	desired.quaternion[3]=1;
	desired.angularRate[0]=0;
	desired.angularRate[1]=0;
	desired.angularRate[2]=0;
										   
	control::MeasuredState measured;
	/*	measured.depth=0;
	measured.linearAcceleration[0]=0;
	measured.linearAcceleration[1]=0;
	measured.linearAcceleration[2]=0;
	measured.magneticField[0]=0;
	measured.magneticField[1]=0;
	measured.magneticField[2]=0;*/
	measured.quaternion[0]=0;
	measured.quaternion[1]=0;
	measured.quaternion[2]=0;
	measured.quaternion[3]=1;
	measured.angularRate[0]=0;
	measured.angularRate[1]=0;
	measured.angularRate[2]=0;

	control::ControllerState state;
	/*	state.angularPGain=10;
	state.angularDGain=1;
	state.inertiaEstimate[0][0]=0.201;
	state.inertiaEstimate[0][1]=0;
	state.inertiaEstimate[0][2]=0;
	state.inertiaEstimate[1][0]=0;
	state.inertiaEstimate[1][1]=1.288;
	state.inertiaEstimate[1][2]=0;
	state.inertiaEstimate[2][0]=0;
	state.inertiaEstimate[2][1]=0;
	state.inertiaEstimate[2][2]=1.288;
	state.depthPGain=0;
	state.speedPGain=0;*/
	state.adaptCtrlRotK=1;
	state.adaptCtrlRotLambda=1;
	state.adaptCtrlRotGamma=1;
	
	state.adaptCtrlParams.resize(12,1);
	state.adaptCtrlParams[0][0]=0.5;
	state.adaptCtrlParams[1][0]=0;
	state.adaptCtrlParams[2][0]=-0.1;
	state.adaptCtrlParams[3][0]=1;
	state.adaptCtrlParams[4][0]=0;
	state.adaptCtrlParams[5][0]=1;
	state.adaptCtrlParams[6][0]=0;
	state.adaptCtrlParams[7][0]=0;
	state.adaptCtrlParams[8][0]=0;
	state.adaptCtrlParams[9][0]=1;
	state.adaptCtrlParams[10][0]=2;
	state.adaptCtrlParams[11][0]=2;

	state.dtMin=0.01;
	state.dtMax=1;

	double act_torques[3];
	double expect_torques[3] = {0,0,0};
	control::AdaptiveRotationalController(&measured, &desired, &state,
										  0.2, act_torques);
	//CHECK_ARRAY_CLOSE(expect_torques,act_torques,3,0.0001);

	//TEST 2
	//	control::DesiredState desired;
	/*	desired.speed=0;
	desired.sidewaysSpeed=0;
	desired.depth=0;*/
	desired.quaternion[0]=0;
	desired.quaternion[1]=0;
	desired.quaternion[2]=0.7071;
	desired.quaternion[3]=0.7071;
	desired.angularRate[0]=0;
	desired.angularRate[1]=0;
	desired.angularRate[2]=0;

										   
	//	control::MeasuredState measured;
	/*	measured.depth=0;
	measured.linearAcceleration[0]=0;
	measured.linearAcceleration[1]=0;
	measured.linearAcceleration[2]=0;
	measured.magneticField[0]=0;
	measured.magneticField[1]=0;
	measured.magneticField[2]=0;*/
	measured.quaternion[0]=0;
	measured.quaternion[1]=0;
	measured.quaternion[2]=0;
	measured.quaternion[3]=1;
	measured.angularRate[0]=0;
	measured.angularRate[1]=0;
	measured.angularRate[2]=0;

	//	control::ControllerState state;
	/*	state.angularPGain=10;
	state.angularDGain=1;
	state.inertiaEstimate[0][0]=0.201;
	state.inertiaEstimate[0][1]=0;
	state.inertiaEstimate[0][2]=0;
	state.inertiaEstimate[1][0]=0;
	state.inertiaEstimate[1][1]=1.288;
	state.inertiaEstimate[1][2]=0;
	state.inertiaEstimate[2][0]=0;
	state.inertiaEstimate[2][1]=0;
	state.inertiaEstimate[2][2]=1.288;
	state.depthPGain=0;
	state.speedPGain=0;*/
	state.adaptCtrlRotK=1;
	state.adaptCtrlRotLambda=1;
	state.adaptCtrlRotGamma=1;
	
	state.adaptCtrlParams.resize(12,1);
	state.adaptCtrlParams[0][0]=0.5;
	state.adaptCtrlParams[1][0]=0;
	state.adaptCtrlParams[2][0]=-0.1;
	state.adaptCtrlParams[3][0]=1;
	state.adaptCtrlParams[4][0]=0;
	state.adaptCtrlParams[5][0]=1;
	state.adaptCtrlParams[6][0]=0;
	state.adaptCtrlParams[7][0]=0;
	state.adaptCtrlParams[8][0]=0;
	state.adaptCtrlParams[9][0]=1;
	state.adaptCtrlParams[10][0]=2;
	state.adaptCtrlParams[11][0]=2;

	state.dtMin=0.01;
	state.dtMax=1;

	//double act_torques[3];
	//double expect_torques[3] = {0,0,-0.7071};
	expect_torques[0] = 0;
	expect_torques[1] = 0;
	expect_torques[2] = 0;
	control::AdaptiveRotationalController(&measured, &desired, &state,
										  0.1, act_torques);
	CHECK_ARRAY_CLOSE(expect_torques,act_torques,3,0.0001);


	/*
    // Testing Yaw Control
    double exp_rotTorques[3] = {0, 0, -3.5497};
    double act_rotTorques[3] = {0};
    control::BongWiePDRotationalController(&measured, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques, act_rotTorques, 3, 0.0001);


    // Testing pitch Control
    double exp_rotTorques2[3] = {0, -2.7872, 0};
    control::MeasuredState measured2 = {0, {0}, {0}, 
                                       {0, 0.2164, 0, 0.9763},
                                       {0}};
    control::BongWiePDRotationalController(&measured2, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques2, act_rotTorques, 3, 0.0001);

    // Testing Roll Control
    double exp_rotTorques3[3] = {0.7692, 0, 0};
    control::MeasuredState measured3 = {0, {0}, {0}, 
                                        {-0.3827, 0, 0, 0.9239},
                                       {0}};
    control::BongWiePDRotationalController(&measured3, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques3, act_rotTorques, 3, 0.0001);*/
}

TEST(doIsOriented)
{
    // Not matching
    control::DesiredState desiredBad = {0, 0, 0, {0.0872, 0, 0, 0.9962}, {0}};

    control::MeasuredState measuredBad = {0, {0}, {0},
                                          {0.2588, 0, 0, 0.9659},
                                          {0}};

    CHECK_EQUAL(false, doIsOriented(&measuredBad, &desiredBad, 0.15));

    // Matching
    control::DesiredState desiredGood = {0, 0, 0, {0.0872, 0, 0, 0.9962}, {0}};
    control::MeasuredState measuredGood = {0, {0}, {0},
                                           {0.173, 0.0858, -0.0151, 0.9811},
                                          {0}};
    
    CHECK_EQUAL(true, doIsOriented(&measuredGood, &desiredGood, 0.15));
}
