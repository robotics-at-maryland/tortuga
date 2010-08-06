/*
 *  ObserverControllerDiscreteTest.cpp
 *  
 *
 *  Created by AUV Club on 2/17/09.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/include/ControlFunctions.h"
#include "math/include/Vector2.h"
#include "math/include/Matrix2.h"

using namespace ram;

TEST(TestDive1To5){
	control::DesiredState desired = {0};
	desired.depth = 5;
		
	control::MeasuredState measured = {0};
    measured.depth = 1;

	control::ControllerState state = {0};

state.depthA4 = math::Matrix4(.9501, -114.3028, -2.8764, 0.0,
                                0.0, 0.4482, 0.0277, 0.0,
                                0.0019, -2.8158, 0.9146, 0.0,
                                -0.0481, 3.5461, -0.2331, 1.0);//Matrix A
state.depthB4 = math::Vector4(114.1389,0.5518, 2.8157, -3.7101); //Vector2 B 
state.depthC4 = math::Vector4(0.0,0.0,0.0,1); //Vector2 C
state.dtMin = .001;
state.dtMax = 1; 

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat4Depth = math::Vector4(0,-1,1,0);

double result = depthObserverController4Discrete(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(11.0612,result,0.0001);
}
