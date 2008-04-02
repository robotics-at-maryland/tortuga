/*
 *  ObserverControllerTests.cpp
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

TEST(TestDive0To1v2)
{
	// K = {9.5358, 16.12}
	// L = {43.525, 461.173}
	// xHat2Depth = {0, 0}
	
	control::DesiredState desired = {0};
	desired.depth = 1;
		
	control::MeasuredState measured = {0};

	control::ControllerState state = {0};
/*{0, //angP gain (not used)
0, //angD gain (not used)
{{0,0,0}, //inertia est (not used)
{0,0,0},
{0,0,0}},
17, //depthP gain
0};, //speedPgain (not used)
		  {{0, 1},
{0, -0.575}},//Matrix2 A
		  {0, 0.05}, //Vector2 B 
		  {1, 0}, //Vector2 C
		  {9.5358, 16.12}, //Vector2 K
		  {43.525, 461.173}, //Vector2 L
			 0 //dt (delta_time)
};*/
state.depthPGain = 17;
state.depthA = math::Matrix2(0,1,0,-.575);
state.depthB = math::Vector2(0, 0.05); //Vector2 B 
state.depthC = math::Vector2(1, 0); //Vector2 C
state.depthK = math::Vector2(9.5358, 16.12); //Vector2 K
state.depthL = math::Vector2(43.525, 461.173); //Vector2 L
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};

depthObserver2(&measured,&desired,&state,&estimated,1);
double result = depthPDController2(&measured,&desired,&state,&estimated);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(result,9.5358,.0001);
}

TEST(Dive1To1v2){
	// K = {9.5358, 16.12}
	// L = {43.525, 461.173}
	// xHat2Depth = {1, 0}
	
	control::DesiredState desired = {0};
	desired.depth = 1;
	
	control::MeasuredState measured = {0};
	measured.depth = 1;
/*{1, //depth
{0},  //accel
{0},  //Mag
{0,0,0,0} //quat (not used)
	{0} //ang rate (not used)
};*/

control::ControllerState state= {0};
/*{0, //angP gain (not used)
	0, //angD gain (not used)
	{{0,0,0}, //inertia est (not used)
	{0,0,0},
	{0,0,0}},
	17, //depthP gain
	0, //speedPgain (not used)
	{{0, 1},
	{0, -0.575}},//Matrix2 A
	{0, 0.05}, //Vector2 B 
	{1, 0}, //Vector2 C
	{9.5358, 16.12}, //Vector2 K
	{43.525, 461.173}, //Vector2 L
	1 //dt (delta time)
};*/
state.depthPGain = 17;
state.depthA = math::Matrix2(0,1,0,-.575);
state.depthB = math::Vector2(0, 0.05); //Vector2 B 
state.depthC = math::Vector2(1, 0); //Vector2 C
state.depthK = math::Vector2(9.5358, 16.12); //Vector2 K
state.depthL = math::Vector2(43.525, 461.173); //Vector2 L
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};
estimated.xHat2Depth = math::Vector2(1,0);
/*{ {1,0} //Vector2 xHat2Depth
};*/

depthObserver2(&measured,&desired,&state,&estimated,1);
double result = depthPDController2(&measured,&desired,&state,&estimated);

//double result = depthObserverController(&measured,&desired,&state,&estimated, 1);

CHECK_CLOSE(result,7.6859,.0001);
}

TEST(TestDive0To1withDiffKandLv2){
	// K = {3.1622, 4.5854}
	// L = {0.22927, 0.026282}
	// xHat2Depth = {0, 0}
	
	control::DesiredState desired = {0};
	desired.depth = 1;
	
	control::MeasuredState measured = {0};

control::ControllerState state = {0};
/*{0, //angP gain (not used)
	0, //angD gain (not used)
	{{0,0,0}, //inertia est (not used)
	{0,0,0},
	{0,0,0}},
	17, //depthP gain
	0, //speedPgain (not used)
	{{0, 1},
	{0, -0.575}},//Matrix2 A
	{0, 0.05}, //Vector2 B 
	{1, 0}, //Vector2 C
	{3.1622, 4.5854}, //Vector2 K
	{0.22927, 0.026282}, //Vector2 L
	0 //dt (delta_time)
};*/
state.depthPGain = 17;
state.depthA = math::Matrix2(0,1,0,-.575);
state.depthB = math::Vector2(0, 0.05); //Vector2 B 
state.depthC = math::Vector2(1, 0); //Vector2 C
state.depthK = math::Vector2(3.1622, 4.5854); //Vector2 K
state.depthL = math::Vector2(0.22927, 0.026282); //Vector2 L
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};

depthObserver2(&measured,&desired,&state,&estimated,1);
double result = depthPDController2(&measured,&desired,&state,&estimated);

//double result = depthObserverController(&measured,&desired,&state,&estimated,1);

CHECK_CLOSE(result,3.1622,.0001);
}


TEST(TestDive0To2v2){
	// K = {3.1622, 4.5854}
	// L = {0.22927, 0.026282}
	// xHat2Depth = {0, 0}
	
	control::DesiredState desired = {0};
	desired.depth = 2;
	
	control::MeasuredState measured = {0};

control::ControllerState state = {0};
/*{0, //angP gain (not used)
	0, //angD gain (not used)
	{{0,0,0}, //inertia est (not used)
	{0,0,0},
	{0,0,0}},
	17, //depthP gain
	0, //speedPgain (not used)
	{{0, 1},
	{0, -0.575}},//Matrix2 A
	{0, 0.05}, //Vector2 B 
	{1, 0}, //Vector2 C
	{3.1622, 4.5854}, //Vector2 K
	{0.22927, 0.026282}, //Vector2 L
	0 //dt (delta_time)
};*/
state.depthPGain = 17;
state.depthA = math::Matrix2(0,1,0,-.575);
state.depthB = math::Vector2(0, 0.05); //Vector2 B 
state.depthC = math::Vector2(1, 0); //Vector2 C
state.depthK = math::Vector2(3.1622, 4.5854); //Vector2 K
state.depthL = math::Vector2(0.22927, 0.026282); //Vector2 L
state.dt = 1; //dt (delta_time)

control::EstimatedState estimated = {math::Vector2(0,0)};

depthObserver2(&measured,&desired,&state,&estimated,1);
double result = depthPDController2(&measured,&desired,&state,&estimated);

//double result2 = depthObserverController(&measured,&desired,&state,&estimated, 1);

CHECK_CLOSE(result,6.3244,.0001);
}

