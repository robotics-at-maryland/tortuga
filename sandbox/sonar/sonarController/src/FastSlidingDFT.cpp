/*
 *  FastSlidingDFT.cpp
 *  sonarController
 *
 *  Created by Tome Capon on 01/20/08.
 *  Based on work by Leo Singer.
 *  Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 */

#include "FastSlidingDFT.h"


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
	delete coefreal;
	delete coefimag;
	delete [] sumreal;
	delete [] sumimag;
	delete [] mag;
	for (int i = 0 ; i < nchannels ; i ++)
	{
		delete [] windowreal[i];
	}
	delete [] windowreal;
}


/** Update the one selected Fourier amplitude using a sliding DFT
 */
void SimpleSlidingDFT::update(adcdata_t * sample)
{
	for (int channel = 0 ; channel < nchannels ; channel ++)
	{
		/*	Step 1: Subtract the old data from the real part sum.
		 *	This removes the x(0) term from the DFT, corresponding to the
		 *	data point that is getting pushed out of the window.
		 *	Since all ADC data is real, we do not touch the imaginary part.
		 */
		
		sumreal[channel] -= windowreal[channel][curidx];
		
		/*	Step 2: Add the new data point in the x(0) position.
		 *	Also store new data in the buffer.
		*/
		
		sumreal[channel] += sample[channel];
		windowreal[channel][curidx] = sample[channel];

		/*	Step 3: Phase-shift the DFT sum.
		 *	Note that after step 2, the point that should be x(N-1) is in the
		 *	x(0) position.  We can correct this simply "rotating" the DFT *backwards*
		 *	one index.  We use a single complex multiplication to achieve this, using a
		 *	constanst factor:
		 *	
		 *	coef = exp(2*pi*i*k/N)
		 *
		 *	Note that the exponent is positive; this causes the backwards shift.
		*/		

		windowreal[channel][curidx] = (adcmath_t) (coefreal * windowreal[channel][curidx] - coefimag * windowimag[channel][curidx]);
		windowimag[channel][curidx] = (adcmath_t) (coefimag * windowreal[channel][curidx] + coefimag * windowreal[channel][curidx]);

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
	coefreal = (adcdata_t) (cos(2 * M_PI * k / N) * ADCDATA_MAXAMPLITUDE);
	coefimag = (adcdata_t) (sin(2 * M_PI * k / N) * ADCDATA_MAXAMPLITUDE);
}


void SimpleSlidingDFT::setupWindow() {
	windowreal = new adcmath_t*[nchannels];
	sumreal = new adcmath_t[nchannels];
	sumimag = new adcmath_t[nchannels];
	mag = new adcmath_t[nchannels];
	for (int i = 0 ; i < nchannels ; i ++)
	{
		windowreal[i] = new adcmath_t[N];
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
