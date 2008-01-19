/*
 *  SimpleSlidingDFT.cpp
 *  sonarController
 *
 *  Created by Leo Singer on 12/10/07.
 *  Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include "SimpleSlidingDFT.h"


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


namespace ram {
namespace sonar {


SimpleSlidingDFT::SimpleSlidingDFT(int nchannels, int k, int N) : SlidingDFT(nchannels, k, N)
{
	setupCoefficients();
	setupWindow();
 }


SimpleSlidingDFT::~SimpleSlidingDFT()
{
	delete [] coefreal;
	delete [] coefimag;
	delete [] sumreal;
	delete [] sumimag;
	delete [] mag;
	for (int i = 0 ; i < nchannels ; i ++)
	{
		delete [] windowreal[i];
		delete [] windowimag[i];
	}
	delete [] windowreal;
	delete [] windowimag;
}


/** Update the one selected Fourier amplitude using a sliding DFT
 */
void SimpleSlidingDFT::update(adcdata_t * sample)
{
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		/*	We subtract window____[channel][firstidx] because both windowreal[i]
		 *	and windowimag[i] are circular buffers.  On the next call of this 
		 *	function, window____[channel][firstidx] will be overwritten with 
		 *	computations from the next sample.
		 */
		
		sumreal[channel] -= windowreal[channel][curidx];
		sumimag[channel] -= windowimag[channel][curidx];
		
		/*	For each sample we receive, we only need to compute one new term in
		 *	the DFT sum.  These two lines together compute 
		 *
		 *		f(N-1) x cos(2 pi k (N - 1) / N) 
		 *
		 *	and
		 *
		 *		f(N-1) x sin(2 pi k (N - 1) / N)
		 *
		 *	which are, respectively, the real and imaginary parts of 
		 *	
		 *		f(N-1) x exp(2 pi i k (N - 1) / N)
		 *	
		 *	which is simply the last term of the sum for F(k).
		 */
		
		windowreal[channel][curidx] = (adcmath_t) coefreal[curidx] * sample[channel];
		windowimag[channel][curidx] = (adcmath_t) coefimag[curidx] * sample[channel];
		
		/*	The next two lines update the real and imaginary part of the complex
		 *	output amplitude.
		 *	
		 *	We add window____[channel][curidx] to sum____[chan constnel] because all 
		 *	of the previous N-1 terms of F(k), numbered 0,1,...,N-3,N-2, were 
		 *	calculated and summed during previous iterations of this function - 
		 *	hence the name "sliding DFT".
		 */
		
		sumreal[channel] += windowreal[channel][curidx];
		sumimag[channel] += windowimag[channel][curidx];
		
		/*	We compute the L1 norm (|a|+|b|) instead of the L2 norm 
		 *	sqrt(a^2+b^2) in order to aovid integer overflow.  Since we are only
		 *	using the magnitude for thresholding, this is an acceptable 
		 *	approximation.
		 */
		
		mag[channel] = abs(sumreal[channel]) + abs(sumimag[channel]);
	}
	
	/*	curidx represents the index into the circular buffers 
	 *	windowreal[channel] and windowimag[channel] at which the just-received
	 *	sample will be added to the DFT sum.
	 */
	
	++curidx;
	if (curidx == N)
		curidx = 0;
}


void SimpleSlidingDFT::setupCoefficients()
{
	coefreal = new adcdata_t[N];
	coefimag = new adcdata_t[N];
	for (int n = 0 ; n < N ; n++)
	{
		coefreal[n] = (adcdata_t) (cos(- 2 * M_PI * n * k / N) * ADCDATA_MAXAMPLITUDE);
		coefimag[n] = (adcdata_t) (sin(- 2 * M_PI * n * k / N) * ADCDATA_MAXAMPLITUDE);
	}
}


void SimpleSlidingDFT::setupWindow() {
	windowreal = new adcmath_t*[nchannels];
	windowimag = new adcmath_t*[nchannels];
	sumreal = new adcmath_t[nchannels];
	sumimag = new adcmath_t[nchannels];
	mag = new adcmath_t[nchannels];
	for (int i = 0 ; i < nchannels ; i ++)
	{
		windowreal[i] = new adcmath_t[N];
		windowimag[i] = new adcmath_t[N];
	}
	purge();
}


void SimpleSlidingDFT::purge()
{
	memset(sumreal, 0, sizeof(*sumreal) * nchannels);
	memset(sumimag, 0, sizeof(*sumimag) * nchannels);
	memset(mag, 0, sizeof(*mag) * nchannels);
	for (int i = 0 ; i < nchannels ; i ++)
	{
		memset(windowreal[i], 0, sizeof(**windowreal) * N);
		memset(windowimag[i], 0, sizeof(**windowimag) * N);
	}
	curidx = 0;
}


adcmath_t SimpleSlidingDFT::getMagL1(int channel) const
{
	assert(channel < nchannels);
	return mag[channel];
}


adcmath_t SimpleSlidingDFT::getReal(int channel) const
{
	assert(channel < nchannels);
	return sumreal[channel];
}


adcmath_t SimpleSlidingDFT::getImag(int channel) const
{
	assert(channel < nchannels);
	return sumimag[channel];
}


} // namespace sonar
} // namespace ram
