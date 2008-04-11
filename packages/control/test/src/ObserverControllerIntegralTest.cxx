/*
 *  ObserverControllerIntegralTest.cpp
 *  
 *
 *  Created by AUV Club on 3/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/include/ControlFunctions.h"
#include "math/include/Vector2.h"
#include "math/include/Matrix2.h"

using namespace ram;

TEST(TestDive0To1v2Integrator)
{
	control::DesiredState desired = {0};
	desired.depth = 2;
		
	control::MeasuredState measured = {0};
    measured.depth = 2.2164;

	control::ControllerState state = {0};

state.depthA4 = math::Matrix4(-1.2, -4204.2, -7.3, 0.0,
                                0.0, -17.4, 1.0, 0.0,
                                0.1, -97.0, -0.6, 0.0,
                                -1.2, -4.2, -7.3, 0.0);//Matrix A
state.depthB4 = math::Vector4(4200.0,17.4, 97.0, 0.0); //Vector2 B 
state.depthC4 = math::Vector4(0.0,0.0,0.0,1); //Vector2 C
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat4Depth = math::Vector4(0,.2162,-.0666,0);

//depthObserver4(&measured,&desired,&state,&estimated,1);
double result = depthObserverController4(&measured,&desired,&state,&estimated,1);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(-.4210275,result,.001);
}

TEST(Dive2To2Integrator){
	control::DesiredState desired = {0};
	desired.depth = 2;
		
	control::MeasuredState measured = {0};
    measured.depth = 1.9565;

	control::ControllerState state = {0};

state.depthA4 = math::Matrix4(-1.2, -4204.2, -7.3, 0.0,
                                0.0, -17.4, 1.0, 0.0,
                                0.1, -97.0, -0.6, 0.0,
                                -1.2, -4.2, -7.3, 0.0);//Matrix A
state.depthB4 = math::Vector4(4200.0,17.4, 97.0, 0.0); //Vector2 B 
state.depthC4 = math::Vector4(0.0,0.0,0.0,1); //Vector2 C
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat4Depth = math::Vector4(0,-0.0435,0.0108,0);

//depthObserver4(&measured,&desired,&state,&estimated,1);
double result = depthObserverController4(&measured,&desired,&state,&estimated,1);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(0.103725,result,0.001);
}

TEST(TestDive0To4v1Integrator){
	control::DesiredState desired = {0};
	desired.depth = 4;
		
	control::MeasuredState measured = {0};
    measured.depth = 3.9526;

	control::ControllerState state = {0};

state.depthA4 = math::Matrix4(-1.2, -4204.2, -7.3, 0.0,
                                0.0, -17.4, 1.0, 0.0,
                                0.1, -97.0, -0.6, 0.0,
                                -1.2, -4.2, -7.3, 0.0);//Matrix A
state.depthB4 = math::Vector4(4200.0,17.4, 97.0, 0.0); //Vector2 B 
state.depthC4 = math::Vector4(0.0,0.0,0.0,1); //Vector2 C
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat4Depth = math::Vector4(0,-0.0435,1.1759,0);

//depthObserver4(&measured,&desired,&state,&estimated,1);
double result = depthObserverController4(&measured,&desired,&state,&estimated,1);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(-8.416,result,.1);
}


TEST(TestDive0To4v2Integrator){
	control::DesiredState desired = {0};
	desired.depth = 4;
		
	control::MeasuredState measured = {0};
    measured.depth = 4.0092;

	control::ControllerState state = {0};

state.depthA4 = math::Matrix4(-1.2, -4204.2, -7.3, 0.0,
                                0.0, -17.4, 1.0, 0.0,
                                0.1, -97.0, -0.6, 0.0,
                                -1.2, -4.2, -7.3, 0.0);//Matrix A
state.depthB4 = math::Vector4(4200.0,17.4, 97.0, 0.0); //Vector2 B 
state.depthC4 = math::Vector4(0.0,0.0,0.0,1); //Vector2 C
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat4Depth = math::Vector4(0,0,-.0042,0);

//depthObserver4(&measured,&desired,&state,&estimated,1);
double result = depthObserverController4(&measured,&desired,&state,&estimated,1);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(0.0307,result,0.001);
}

TEST(TestDive0To4v3Integrator){
	control::DesiredState desired = {0};
	desired.depth = 4;
		
	control::MeasuredState measured = {0};
    measured.depth = 0.0707;

	control::ControllerState state = {0};

state.depthA4 = math::Matrix4(-1.2, -4204.2, -7.3, 0.0,
                                0.0, -17.4, 1.0, 0.0,
                                0.1, -97.0, -0.6, 0.0,
                                -1.2, -4.2, -7.3, 0.0);//Matrix A
state.depthB4 = math::Vector4(4200.0,17.4, 97.0, 0.0); //Vector2 B 
state.depthC4 = math::Vector4(0.0,0.0,0.0,1); //Vector2 C
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat4Depth = math::Vector4(0,-0.5158,-5.3307,0);

//depthObserver4(&measured,&desired,&state,&estimated,1);
double result = depthObserverController4(&measured,&desired,&state,&estimated,1);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(41.147,result,0.1);
}
