/**
 * @file FastSlidingDFT.h
 *
 * @author Tom Capon
 * Based on work by Leo Singer.
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 *
 * TODO: Currently nonfunctioning.  Fix based on SDFTSpectrum.h
 */


#ifndef _RAM_SONAR_FASTSLIDINGDFT_H
#define _RAM_SONAR_FASTSLIDINGDFT_H


#include "../Sonar.h"
#include "SlidingDFT.h"


namespace ram {
namespace sonar {

template<ADC, int nchannels, int k, int N>
class FastSlidingDFT : public SlidingDFT<ADC> {

public:
	FastSlidingDFT()
	{
		coefreal = cos(2 * M_PI * k / N) * ADC::SIGNED_MAX;
		coefimag = sin(2 * M_PI * k / N) * ADC::SIGNED_MAX;
		purge();
	}
	
	virtual void purge()
	{
		bzero(sumreal, sizeof(*sumreal) * nchannels);
		bzero(sumimag, sizeof(*sumimag) * nchannels);
		bzero(mag, sizeof(*mag) * nchannels);
		bzero(window, sizeof(**window) * N * nchannels);
		curidx = 0;
	}
	
	virtual void update(const typename ADC::SIGNED* sample)
	{
		for (int channel = 0 ; channel < nchannels ; channel ++)
		{
			
			//	After http://www.comm.toronto.edu/~dimitris/ece431/slidingdft.pdf
			
			/*	Step 1: Subtract the old data from the real part sum.
			 *	This removes the x(0) term from the DFT, corresponding to the
			 *	data point that is getting pushed out of the window.
			 *	Since all ADC data is real, we do not touch the imaginary part.
			 */
			
			sumreal[channel] -= window[channel][curidx];
			
			/*	Step 2: Add the new data point in the x(0) position.
			 *	Also store new data in the buffer.
			 */
			
			sumreal[channel] += sample[channel];
			window[channel][curidx] = sample[channel];
			
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
			
			typename ADC::DOUBLE_WIDE::SIGNED tmp = coefreal * sumreal[channel] - coefimag * sumimag[channel];
			sumimag[channel] = coefreal * sumimag[channel] + coefimag * sumreal[channel];
			sumreal[channel] = tmp;
			
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
	
	
	virtual typename ADC::DOUBLE_WIDE::SIGNED getMagL1(int channel) const {return mag[channel];}
	virtual typename ADC::DOUBLE_WIDE::SIGNED getReal(int channel) const {return sumreal[channel];}
	virtual typename ADC::DOUBLE_WIDE::SIGNED getImag(int channel) const {return sumimag[channel];}
	virtual int getCountChannels() const {return nchannels;}
	virtual int getFourierIndex() const {return k;}
	virtual int getWindowSize() const {return N;}
	
private:
	typename ADC::SIGNED  coefreal, coefimag;
	typename ADC::SIGNED window[nchannels][N];
	typename ADC::DOUBLE_WIDE::SIGNED sumreal[nchannels], sumimag[nchannels], mag[nchannels];
	
	int curidx;
};


} // namespace sonar
} // namespace ram

#endif

