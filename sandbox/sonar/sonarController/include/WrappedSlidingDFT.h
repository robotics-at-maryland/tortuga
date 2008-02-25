/*
 *  WrappedSlidingDFT.h
 *  sonarController
 *
 *  Created by Leo Singer on 02/25/08.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 *  Wrapper for dft.h, primarily for the purpose of unit testing.
 * 
 */

#ifndef WRAPPEDSLIDINGDFT_H
#define WRAPPEDSLIDINGDFT_H


#include "Sonar.h"
#include "SlidingDFT.h"
#include "dft.h"


namespace ram {
namespace sonar {


class WrappedSlidingDFT : public SlidingDFT {

public:
	WrappedSlidingDFT(int nchannels, int k, int N);
	~WrappedSlidingDFT();
	virtual void purge();
	virtual void update(adcdata_t * sample);
	virtual adcmath_t getMagL1(int channel) const;
	virtual adcmath_t getReal(int channel) const;
	virtual adcmath_t getImag(int channel) const;
	
private:
	sliding_dft_t *dft;
};


} // namespace sonar
} // namespace ram


#endif
