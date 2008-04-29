/*
 *  SlidingDFT.h
 *  sonarController
 *
 *  Created by Leo Singer on 12/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Sliding Discrete Fourier Transform (DFT).  Abstract base clase
 *  allowing for platform optimized or special purpose implementations.
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
	virtual void purge() =0;
	virtual void update(const adcdata_t *) =0;
	virtual adcmath_t getReal(int channel) const =0;
	virtual adcmath_t getImag(int channel) const =0;
	virtual adcmath_t getMagL1(int channel) const =0;
	virtual adcmath_t getMagL2(int channel) const;
	virtual adcmath_t getUnity() const;
	virtual float getPhase(int channel) const;
	virtual int getCountChannels() const =0;
	virtual int getFourierIndex() const =0;
	virtual int getWindowSize() const =0;
	virtual void println() const;

};


} // namespace sonar
} // namespace ram


#endif
