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


template<int nchannels, int k, int N>
WrappedSlidingDFT<nchannels, k, N>::WrappedSlidingDFT()
{
	dft = dft_alloc(nchannels, k, N);
}


template<int nchannels, int k, int N>
WrappedSlidingDFT<nchannels, k, N>::~WrappedSlidingDFT()
{
	dft_free(dft);
}


/** Update the one selected Fourier amplitude using a sliding DFT
 */
template<int nchannels, int k, int N>
void WrappedSlidingDFT<nchannels, k, N>::update(adcdata_t * sample)
{
	dft_update(dft, sample);
}


template<int nchannels, int k, int N>
void WrappedSlidingDFT<nchannels, k, N>::purge()
{
	dft_purge(dft);
}


template<int nchannels, int k, int N>
adcmath_t WrappedSlidingDFT<nchannels, k, N>::getMagL1(int channel) const
{
	assert(channel < nchannels);
	return dft->mag[channel];
}


template<int nchannels, int k, int N>
adcmath_t WrappedSlidingDFT<nchannels, k, N>::getReal(int channel) const
{
	assert(channel < nchannels);
	return dft->re[channel];
}


template<int nchannels, int k, int N>
adcmath_t WrappedSlidingDFT<nchannels, k, N>::getImag(int channel) const
{
	assert(channel < nchannels);
	return dft->im[channel];
}


} // namespace sonar
} // namespace ram
