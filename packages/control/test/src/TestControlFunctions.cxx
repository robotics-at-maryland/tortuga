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
#include "math/test/include/MathChecks.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector3.h"

using namespace ram;

TEST(TranslationController)
{
    // Depth test
    control::DesiredState desired;
    memset(&desired, 0, sizeof(control::DesiredState));
    desired.depth = 1;

    // At 0 depth, pitched down 45 degrees
    control::MeasuredState measured = {0};
    math::Quaternion orien(math::Degree(45), math::Vector3::UNIT_Y);
    memcpy(measured.quaternion, orien.ptr(), sizeof(measured.quaternion));

    control::ControllerState state = {0};
    state.depthPGain = 1;
    state.depthControlType = 1; // P controller

    control::EstimatedState estimated;
    memset(&estimated, 0, sizeof(control::EstimatedState));

    // The expected
    math::Vector3 expected(0.707106781, 0, -0.707106781);

    // Run the controller
    math::Vector3 result;
    translationalController(&measured, &desired, &state, &estimated, 0.1,
                            result.ptr());

    CHECK_CLOSE(expected, result, 0.0001);
    
    // Speed test
    desired.depth = 0;
    desired.speed = 1;

    // At 0 depth, pitched down 45 degrees
    // Keeping old measured state
    
    state.depthPGain = 0;
    state.speedPGain = 1;

    // The expected
    result = math::Vector3::ZERO;
    expected = math::Vector3(0.707106781, 0, 0.707106781);

    // Run the controller
    translationalController(&measured, &desired, &state, &estimated, 0.1,
                            result.ptr());

    CHECK_CLOSE(expected, result, 0.0001);


    // Speed test with yaw
    desired.depth = 0;
    desired.speed = 1;

    // At 0 depth, pitch 0, yawed 45 degrees
    orien = math::Quaternion(math::Degree(45), math::Vector3::UNIT_Z);
    memcpy(measured.quaternion, orien.ptr(), sizeof(measured.quaternion));
    
    state.depthPGain = 0;
    state.speedPGain = 1;

    // The expected
    result = math::Vector3::ZERO;
    expected = math::Vector3(1, 0, 0);

    // Run the controller
    translationalController(&measured, &desired, &state, &estimated, 0.1,
                            result.ptr());

    CHECK_CLOSE(expected, result, 0.0001);

    
    // Combine
    desired.depth = 1;

    // At 0 depth, pitched down 45 degrees
    orien = math::Quaternion(math::Degree(45), math::Vector3::UNIT_Y);
    memcpy(measured.quaternion, orien.ptr(), sizeof(measured.quaternion));
    
    // Turn on Depth
    state.depthPGain = 1;

    // The expected
    result = math::Vector3::ZERO;
    expected = math::Vector3(0.707106781 * 2, 0, 0);

    // Run the controller
    translationalController(&measured, &desired, &state, &estimated, 0.1,
                            result.ptr());

    CHECK_CLOSE(expected, result, 0.0001);
}

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

	control::EstimatedState estimated;


    // Testing Yaw Control
    double exp_rotTorques[3] = {0, 0, -3.5497};
    double act_rotTorques[3] = {0};
    control::rotationalPDController(&measured, 
				    &desired, 
				    &state,
				    &estimated,
                                    1, 
				    act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques, act_rotTorques, 3, 0.0001);


    // Testing pitch Control
    double exp_rotTorques2[3] = {0, -2.7872, 0};
    control::MeasuredState measured2 = {0, {0}, {0}, 
                                       {0, 0.2164, 0, 0.9763},
                                       {0}};
    control::rotationalPDController(&measured2, 
				    &desired, 
				    &state,
				    &estimated,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques2, act_rotTorques, 3, 0.0001);

    // Testing Roll Control
    double exp_rotTorques3[3] = {0.7692, 0, 0};
    control::MeasuredState measured3 = {0, {0}, {0}, 
                                        {-0.3827, 0, 0, 0.9239},
                                       {0}};
    control::rotationalPDController(&measured3, 
				    &desired, 
				    &state,
				    &estimated,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques3, act_rotTorques, 3, 0.0001);
}

TEST(GyroBiasObserverController){
  control::DesiredState desired;
  control::MeasuredState measured;
  control::ControllerState controller;
  control::EstimatedState estimated;

  //case 1
  //perfect estimates, no orientation/rate error
  desired.quaternion[0]=0;
  desired.quaternion[1]=0;
  desired.quaternion[2]=0;
  desired.quaternion[3]=1;
  desired.angularRate[0]=0;
  desired.angularRate[1]=0;
  desired.angularRate[2]=0;
  
  measured.quaternion[0]=0;
  measured.quaternion[1]=0;
  measured.quaternion[2]=0;
  measured.quaternion[3]=1;
  measured.angularRate[0]=0;
  measured.angularRate[1]=0;
  measured.angularRate[2]=0;

  controller.dtMin=0.1;
  controller.dtMax=2;
  controller.gyroObsGain=1;

  estimated.qhat.x=0;
  estimated.qhat.y=0;
  estimated.qhat.z=0;
  estimated.qhat.w=1;
  estimated.what[0]=0;
  estimated.what[1]=0;
  estimated.what[2]=0;
  estimated.bhat[0]=0;
  estimated.bhat[1]=0;
  estimated.bhat[2]=0;
  estimated.dqhat.x=0;
  estimated.dqhat.y=0;
  estimated.dqhat.z=0;
  estimated.dqhat.w=1;
  estimated.dbhat[0]=0;
  estimated.dbhat[1]=0;
  estimated.dbhat[2]=0;


  double act_torques[3] = {0,0,0};
  control::rotationalGyroObsPDController(&measured, &desired, &controller, &estimated, 1, act_torques);

  math::Quaternion qhatExp(0,0,0,1);
  CHECK_CLOSE(qhatExp,estimated.qhat,0.0002);



  //case 3
  //horribly confusing case
  desired.quaternion[0]=0;
  desired.quaternion[1]=0;
  desired.quaternion[2]=0;
  desired.quaternion[3]=1;
  desired.angularRate[0]=0;
  desired.angularRate[1]=0;
  desired.angularRate[2]=0;
  
  measured.quaternion[0]=-0.3005;
  measured.quaternion[1]=0.9016;
  measured.quaternion[2]=0.3005;
  measured.quaternion[3]=0.0805;
  measured.angularRate[0]=1;
  measured.angularRate[1]=2;
  measured.angularRate[2]=1;

  controller.dtMin=0.1;
  controller.dtMax=2;
  controller.gyroObsGain=1;

  estimated.qhat.x=0;
  estimated.qhat.y=0;
  estimated.qhat.z=0;
  estimated.qhat.w=1;
  estimated.what[0]=0;
  estimated.what[1]=0;
  estimated.what[2]=0;
  estimated.bhat[0]=0;
  estimated.bhat[1]=0;
  estimated.bhat[2]=0;
  estimated.dqhat.x=0;
  estimated.dqhat.y=0;
  estimated.dqhat.z=0;
  estimated.dqhat.w=0;
  estimated.dbhat[0]=0;
  estimated.dbhat[1]=0;
  estimated.dbhat[2]=0;


  double act_torques3[3] = {0,0,0};
  control::rotationalGyroObsPDController(&measured, &desired, &controller, &estimated, 1, act_torques3);


  math::Quaternion qhatoldExp3(0, 0, 0, 1);
  CHECK_CLOSE(qhatoldExp3,controller.qhatold,0.0002);

  math::Quaternion dqhatoldExp3(0, 0, 0, 0);
  CHECK_CLOSE(dqhatoldExp3,controller.dqhatold,0.0002);

  math::Quaternion dqhatExp3(-1.1991,1.2506,0.1152,0);
  CHECK_CLOSE(dqhatExp3,estimated.dqhat,0.0002);

  math::Quaternion qhatExp3(-0.4527,0.4722,0.0435,0.7551);
  CHECK_CLOSE(qhatExp3,estimated.qhat,0.0002);
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

	control::EstimatedState estimated;

	double act_torques[3];
	double expect_torques[3] = {0,0,0};
	control::adaptiveRotationalController(&measured, &desired, &state, &estimated, 0.2, act_torques);
	CHECK_ARRAY_CLOSE(expect_torques,act_torques,3,0.0001);

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
	expect_torques[2] = 0.7071;
	control::adaptiveRotationalController(&measured, &desired, &state, &estimated, 0.1, act_torques);
	CHECK_ARRAY_CLOSE(expect_torques,act_torques,3,0.0001);

	//TEST 3
	//	control::DesiredState desired;
	/*	desired.speed=0;
	desired.sidewaysSpeed=0;
	desired.depth=0;*/
	desired.quaternion[0]=0.7071;
	desired.quaternion[1]=0;
	desired.quaternion[2]=0;
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
	expect_torques[0] = 0.9192;
	expect_torques[1] = 0;
	expect_torques[2] = 0;
	control::adaptiveRotationalController(&measured, &desired, &state, &estimated,  0.3, act_torques);
	CHECK_ARRAY_CLOSE(expect_torques,act_torques,3,0.01);


	//TEST 4
	//	control::DesiredState desired;
	/*	desired.speed=0;
	desired.sidewaysSpeed=0;
	desired.depth=0;*/
	desired.quaternion[0]=0.7071;
	desired.quaternion[1]=0;
	desired.quaternion[2]=0;
	desired.quaternion[3]=0.7071;
	desired.angularRate[0]=-0.2;
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
	expect_torques[0] = 0.4582;
	expect_torques[1] = 0;
	expect_torques[2] = 0.0142;
	control::adaptiveRotationalController(&measured, &desired, &state, &estimated,  0.05, act_torques);
	CHECK_ARRAY_CLOSE(expect_torques,act_torques,3,0.01);



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
