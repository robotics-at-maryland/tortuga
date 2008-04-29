/*
 *  FastSlidingDFT.h
 *  sonarController
 *
 *  Created by Tom Capon on 1/20/08.
 *  Based on work by Leo Singer.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */


#ifndef FASTSLIDINGDFT_H
#define FASTSLIDINGDFT_H


#include "Sonar.h"
#include "SlidingDFT.h"


namespace ram {
namespace sonar {

template<int nchannels, int k, int N>
class FastSlidingDFT : public SlidingDFT {

public:
	FastSlidingDFT();
	virtual void purge();
	virtual void update(const adcdata_t *);
	virtual adcmath_t getMagL1(int channel) const;
	virtual adcmath_t getReal(int channel) const;
	virtual adcmath_t getImag(int channel) const;
	virtual int getCountChannels() const {return nchannels;}
	virtual int getFourierIndex() const {return k;}
	virtual int getWindowSize() const {return N;}
	
private:
	void setupCoefficients();
	void setupWindow();
	
	adcdata_t coefreal, coefimag;
	adcdata_t window[nchannels][N];
	adcmath_t sumreal[nchannels], sumimag[nchannels], mag[nchannels];
	
	int curidx;
};


} // namespace sonar
} // namespace ram

#endif

