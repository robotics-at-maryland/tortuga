/*
 *  SlidingDFT.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 1/19/08.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "SlidingDFT.h"


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


namespace ram {
namespace sonar {


SlidingDFT::SlidingDFT(int nchannels, int k, int N) 
	: nchannels(nchannels), k(k), N(N) {}


SlidingDFT::~SlidingDFT() {}


adcmath_t SlidingDFT::getMagL2(int channel) const
{
	assert(channel < nchannels);
	adcmath_t re = getReal(channel);
	adcmath_t im = getImag(channel);
	return (adcmath_t) sqrt(double(re) * re + double(im) * im);
}


adcmath_t SlidingDFT::getUnity() const
{
	return N * ADCDATA_MAXAMPLITUDE * ADCDATA_MAXAMPLITUDE;
}


float SlidingDFT::getPhase(int channel) const
{
	assert(channel < nchannels);
	return atan2f(getImag(channel), getReal(channel));
}


int SlidingDFT::getCountChannels() const
{
	return nchannels;
}


int SlidingDFT::getFourierIndex() const
{
	return k;
}


int SlidingDFT::getWindowSize() const
{
	return N;
}


void SlidingDFT::println() const
{
	for (int channel = 0 ; channel < nchannels ; channel ++)
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
