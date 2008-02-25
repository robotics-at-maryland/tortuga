/*
 *  WrappedSlidingDFT.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 02/25/08.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */


#include "WrappedSlidingDFT.h"


#include <assert.h>


namespace ram {
namespace sonar {


WrappedSlidingDFT::WrappedSlidingDFT(int nchannels, int k, int N) : SlidingDFT(nchannels, k, N)
{
	dft = dft_alloc(nchannels, k, N);
}


WrappedSlidingDFT::~WrappedSlidingDFT()
{
	dft_free(dft);
}


/** Update the one selected Fourier amplitude using a sliding DFT
 */
void WrappedSlidingDFT::update(adcdata_t * sample)
{
	dft_update(dft, sample);
}


void WrappedSlidingDFT::purge()
{
	dft_purge(dft);
}


adcmath_t WrappedSlidingDFT::getMagL1(int channel) const
{
	assert(channel < nchannels);
	return dft->mag[channel];
}


adcmath_t WrappedSlidingDFT::getReal(int channel) const
{
	assert(channel < nchannels);
	return dft->re[channel];
}


adcmath_t WrappedSlidingDFT::getImag(int channel) const
{
	assert(channel < nchannels);
	return dft->im[channel];
}


} // namespace sonar
} // namespace ram
