/**
 * @file TiledSlidingDFT.h
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 *  
 * Sliding discrete Fourier transform optimized for N divisible by k.
 * Called a "tiled" DFT because the sine wave sin(2*pi*k*n/N) tiles perfectly
 * across the fourier window of length N.
 *
 * Implemented as a sliding dot product.  Has the special property of phase 
 * coherence.
 * 
 */

#ifndef _RAM_SONAR_TILEDSLIDINGDFT_H
#define _RAM_SONAR_TILEDSLIDINGDFT_H

#include "../Sonar.h"
#include "SlidingDFT.h"
#include <strings.h>
#include <cassert>

namespace ram {
namespace sonar {


template<typename ADC, int nchannels, int k, int N>
class TiledSlidingDFT : public SlidingDFT<ADC> {
public:
	TiledSlidingDFT()
	{
		assert(N % k == 0);
		for (int n = 0 ; n < N ; n++)
		{
			coefreal[n] = (typename ADC::SIGNED)
				(cos(- 2 * M_PI * n * k / N) * ADC::SIGNED_MAX());
			coefimag[n] = (typename ADC::SIGNED)
				(sin(- 2 * M_PI * n * k / N) * ADC::SIGNED_MAX());
		}
		purge();
	}
	
	
	virtual void purge()
	{
		bzero(sumreal, sizeof(*sumreal) * nchannels);
		bzero(sumimag, sizeof(*sumimag) * nchannels);
		bzero(mag, sizeof(*mag) * nchannels);
		bzero(windowreal, sizeof(**windowreal) * N * nchannels);
		bzero(windowimag, sizeof(**windowimag) * N * nchannels);
		curidx = 0;
	}
	
	
	virtual void update(const typename ADC::SIGNED * sample)
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
			
			windowreal[channel][curidx] = (typename ADC::DOUBLE_WIDE::SIGNED) coefreal[curidx] * sample[channel];
			windowimag[channel][curidx] = (typename ADC::DOUBLE_WIDE::SIGNED) coefimag[curidx] * sample[channel];
			
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
	
	
	virtual typename ADC::DOUBLE_WIDE::SIGNED getMagL1(int channel) const {return mag[channel];}
	virtual typename ADC::DOUBLE_WIDE::SIGNED getReal(int channel) const {return sumreal[channel];}
	virtual typename ADC::DOUBLE_WIDE::SIGNED getImag(int channel) const {return sumimag[channel];}
	virtual int getCountChannels() const {return nchannels;}
	virtual int getFourierIndex() const {return k;}
	virtual int getWindowSize() const {return N;}
	
private:
	typename ADC::SIGNED coefreal[N], coefimag[N];
	typename ADC::DOUBLE_WIDE::SIGNED windowreal[nchannels][N], windowimag[nchannels][N];
	typename ADC::DOUBLE_WIDE::SIGNED sumreal[nchannels], sumimag[nchannels], mag[nchannels];
	int curidx;
	
};


} // namespace sonar
} // namespace ram


#endif
