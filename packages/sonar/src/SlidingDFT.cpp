/**
 * @file SlidingDFT.cpp
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "dft/SlidingDFT.h"


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


namespace ram {
namespace sonar {


adcmath_t SlidingDFT::getMagL2(int channel) const
{
	adcmath_t re = getReal(channel);
	adcmath_t im = getImag(channel);
	return (adcmath_t) sqrt(double(re) * re + double(im) * im);
}


adcmath_t SlidingDFT::getUnity() const
{
	return getWindowSize() * ADCDATA_MAXAMPLITUDE * ADCDATA_MAXAMPLITUDE;
}


float SlidingDFT::getPhase(int channel) const
{
	assert(channel < getCountChannels());
	return atan2f(getImag(channel), getReal(channel));
}


void SlidingDFT::println() const
{
	for (int channel = 0 ; channel < getCountChannels() ; channel ++)
	{
		printf("Channel %2d: %12d + %12d i  :  MagL1: %12d : MagL2: %d\n",
			channel,
			getReal(channel),
			getImag(channel),
			getMagL1(channel),
			getMagL2(channel)
		);
	}
}


} // namespace sonar
} // namespace ram
