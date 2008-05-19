/**
 * @file WrappedSlidingDFT.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 * Wrapper for dft.h, primarily for the purpose of unit testing.
 * 
 */

#ifndef _RAM_SONAR_WRAPPEDSLIDINGDFT_H
#define _RAM_SONAR_WRAPPEDSLIDINGDFT_H


#include "../Sonar.h"
#include "SlidingDFT.h"
#include "dft.h"


namespace ram {
namespace sonar {


template<int nchannels, int k, int N>
class WrappedSlidingDFT : public SlidingDFT {

public:
	WrappedSlidingDFT() {dft = dft_alloc(nchannels, k, N);}
	~WrappedSlidingDFT() {dft_free(dft);}
	virtual void purge() {dft_purge(dft);}
	virtual void update(const adcdata_t * sample) {dft_update(dft, sample);}
	virtual adcmath_t getMagL1(int channel) const {return dft->mag[channel];}
	virtual adcmath_t getReal(int channel) const {return dft->re[channel];}
	virtual adcmath_t getImag(int channel) const {return dft->im[channel];}
	virtual int getCountChannels() const {return nchannels;}
	virtual int getFourierIndex() const {return k;}
	virtual int getWindowSize() const {return N;}
	
private:
	sliding_dft_t *dft;
};


} // namespace sonar
} // namespace ram


#endif
