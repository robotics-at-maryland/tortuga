/*
 *  Sonar.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 11/29/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "Sonar.h"


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
