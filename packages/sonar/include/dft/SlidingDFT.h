/**
 * @file SlidingDFT.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Sliding Discrete Fourier Transform (DFT).  Abstract base clase
 * allowing for platform optimized or special purpose implementations.
 * 
 */

#ifndef SLIDINGDFT_H
#define SLIDINGDFT_H


#include "../Sonar.h"


#include <math.h>
#include <stdio.h>


namespace ram {
namespace sonar {


template<typename ADC>
class SlidingDFT {
public:
	virtual ~SlidingDFT() {}
	virtual void purge() =0;
	virtual void update(const typename ADC::SIGNED *) =0;
	virtual typename ADC::DOUBLE_WIDE::SIGNED getReal(int channel) const =0;
	virtual typename ADC::DOUBLE_WIDE::SIGNED getImag(int channel) const =0;
	virtual typename ADC::DOUBLE_WIDE::SIGNED getMagL1(int channel) const =0;
	virtual typename ADC::DOUBLE_WIDE::SIGNED getMagL2(int channel) const
	{
		typename ADC::DOUBLE_WIDE::SIGNED re = getReal(channel);
		typename ADC::DOUBLE_WIDE::SIGNED im = getImag(channel);
		return (typename ADC::DOUBLE_WIDE::SIGNED) sqrt(double(re) * re + double(im) * im);
	};
	virtual typename ADC::DOUBLE_WIDE::SIGNED getUnity() const
	{
		return getWindowSize() * ADCDATA_MAXAMPLITUDE * ADCDATA_MAXAMPLITUDE;
	}
	virtual float getPhase(int channel) const
	{
		assert(channel < getCountChannels());
		return atan2f(getImag(channel), getReal(channel));
	}
	virtual int getCountChannels() const =0;
	virtual int getFourierIndex() const =0;
	virtual int getWindowSize() const =0;
	virtual void println() const
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
};


} // namespace sonar
} // namespace ram


#endif
