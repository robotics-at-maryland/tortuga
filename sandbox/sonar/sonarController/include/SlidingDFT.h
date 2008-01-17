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


namespace ram {
namespace sonar {


template<class DERIVED>
class SlidingDFT {

public:
	SlidingDFT(int nchannels, int k, int N) : nchannels(nchannels), k(k), N(N) {}
	virtual void purge() =0;
	virtual void update(adcdata_t * sample) =0;
	virtual adcmath_t getReal(int channel) const =0;
	virtual adcmath_t getImag(int channel) const =0;
	virtual adcmath_t getMagL1(int channel) const =0;
	
	adcmath_t getMagL2(int channel)
	{
		adcmath_t re = static_cast<DERIVED*>(this)->getReal(channel);
		adcmath_t im = static_cast<DERIVED*>(this)->getImag(channel);
		return sqrt(float(re) * re + float(im) * im);
	};
	
	adcmath_t getUnity() const { return N * ADCDATA_MAXAMPLITUDE * ADCDATA_MAXAMPLITUDE; }
	
	float getPhase(int channel) const
	{
		return atan2f(static_cast<DERIVED*>(this)->getImag(),
			static_cast<DERIVED*>(this)->getReal());
	};
	
	int getCountChannels() const { return nchannels; };
	
	int getFourierIndex() const { return k; };
	
	int getWindowSize() const { return N; };

protected:
	int nchannels, k, N;
	
};


} // namespace sonar
} // namespace ram


#endif
