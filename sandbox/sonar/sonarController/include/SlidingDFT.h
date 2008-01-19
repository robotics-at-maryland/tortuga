/*
 *  SlidingDFT.h
 *  sonarController
 *
 *  Created by Leo Singer on 12/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#ifndef SLIDINGDFT_H
#define SLIDINGDFT_H


#include "Sonar.h"


#include <math.h>
#include <stdio.h>


namespace ram {
namespace sonar {


class SlidingDFT {

public:
	SlidingDFT(int nchannels, int k, int N) : nchannels(nchannels), k(k), N(N) {}
	virtual ~SlidingDFT() {}
	virtual void purge() =0;
	virtual void update(adcdata_t * sample) =0;
	virtual adcmath_t getReal(int channel) const =0;
	virtual adcmath_t getImag(int channel) const =0;
	virtual adcmath_t getMagL1(int channel) const =0;
	
	virtual adcmath_t getMagL2(int channel) const
	{
		adcmath_t re = getReal(channel);
		adcmath_t im = getImag(channel);
		return (adcmath_t) sqrt(double(re) * re + double(im) * im);
	};
	
	virtual adcmath_t getUnity() const { return N * ADCDATA_MAXAMPLITUDE * ADCDATA_MAXAMPLITUDE; }
	
	virtual float getPhase(int channel) const
	{
		return atan2f(getImag(channel), getReal(channel));
	};
	
	virtual int getCountChannels() const { return nchannels; };
	
	virtual int getFourierIndex() const { return k; };
	
	virtual int getWindowSize() const { return N; };
	
	virtual void println() const
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

protected:
	int nchannels, k, N;
	
};


} // namespace sonar
} // namespace ram


#endif
