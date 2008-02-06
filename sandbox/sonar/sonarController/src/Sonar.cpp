/*
 *  Sonar.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/29/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "Sonar.h"


#include <assert.h>


namespace ram {
namespace sonar {


int gcd(int a, int b)
{
	assert(a > 0 && b > 0);
	if (b == 0)
		return a;
	else
		return gcd(b, a % b);
}


template<class T>
int8_t compare(T a, T b)
{
	if (a > b)
		return 1;
	if (b < a)
		return -1;
	return 0;
}


template<class T>
int8_t sign(T a)
{
	return compare(a, 0);
}


} // namespace sonar
} // namespace ram
