/*
 *  FastSlidingDFT.h
 *  sonarController
 *
 *  Created by Tom Capon on 1/20/08.
 *  Based on work by Leo Singer.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

/* This class contains errors that prevent it from compiling.
 * No time to fix it now, so just tell the comiler to skip it.
 */
#if false


#ifndef FASTSLIDINGDFT_H
#define FASTSLIDINGDFT_H


#include "Sonar.h"
#include "SlidingDFT.h"


namespace ram {
namespace sonar {


class FastSlidingDFT : public SlidingDFT {

public:
	FastSlidingDFT(int nchannels, int k, int N);
	~FastSlidingDFT();
	virtual void purge();
	virtual void update(adcdata_t * sample);
	virtual adcmath_t getMagL1(int channel) const;
	virtual adcmath_t getReal(int channel) const;
	virtual adcmath_t getImag(int channel) const;
	
private:
	void setupCoefficients();
	void setupWindow();
	
	adcdata_t coefreal, coefimag;
	adcmath_t **windowreal;
	adcmath_t *sumreal, *sumimag, *mag;
	
	int curidx;
};


} // namespace sonar
} // namespace ram


#endif

#endif

